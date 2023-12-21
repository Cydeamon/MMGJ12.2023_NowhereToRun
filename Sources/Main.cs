using System;
using Godot;
using NowhereToRun.Sources.Characters;

namespace NowhereToRun.Sources;

public partial class Main : Node2D
{
    /***************************************************************************/
    /***************************** Game properties *****************************/

    public enum LevelIntroStatus
    {
        LEVEL_STARTED,
        RUNNING_UP,
        STANDING,
        MESSAGE_READY,
        MESSAGE_GO
    }
    
    private static LevelIntroStatus levelIntroStatus;
    private static bool isGameStarted = false;
    private static bool isGamePaused = true;
    private int score = 0;
    private int enemiesInitialNumber = 50;
    private int enemiesLeft = 50;
    [Export] private int enemySpawnTimeMin = 250;
    [Export] private int enemySpawnTimeMax = 1500;
    [Export] public float GrenadeEnemySpawnChance = 10;
    private ulong nextEnemySpawnTime;
    protected readonly Random random = new Random();
    private AudioStream LevelStartMusic = GD.Load<AudioStream>("res://Assets/Music/LevelStart.wav");
    private AudioStream LevelMusic = GD.Load<AudioStream>("res://Assets/Music/Gameplay.wav");

    /***************************************************************************/
    /********************************** Nodes **********************************/

    public Control Menu;
    public Node2D Level;
    public Node2D HUD;
    public Node ProjectilesContainer;
    public AudioStreamPlayer MenuPlayer;
    public AudioStreamPlayer GameplayPlayer;
    public Area2D EnemiesSpawnArea;
    public CollisionShape2D EnemiesSpawnAreaCollision;
    public RectangleShape2D EnemiesSpawnAreaCollisionShape;
    public Player Player;
    public Node2D LevelStartPlayerStopPoint;
    public Node2D MessageSprite;
    public Node2D MessageGoSprite;
    public Node2D MessageReadySprite;
    public Node2D MessageLevelCompleteSprite;
    
    
    /***************************************************************************/
    /***************************** Global game data ****************************/

    public static bool IsGameStarted() => isGameStarted;
    public static bool IsGamePaused() => isGamePaused;
    public static LevelIntroStatus GetLevelIntroStatus() => levelIntroStatus;

    /***************************************************************************/
    /********************************* Methods *********************************/

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        // Init nodes
        Menu = GetNode<Control>("Menu");
        Level = GetNode<Node2D>("Level");
        HUD = GetNode<Node2D>("HUD");
        ProjectilesContainer = GetNode("Level/Projectiles");
        MenuPlayer = GetNode<AudioStreamPlayer>("MenuPlayer");
        GameplayPlayer = GetNode<AudioStreamPlayer>("GameplayPlayer");
        EnemiesSpawnArea = GetNode<Area2D>("Level/EnemiesSpawnArea");
        EnemiesSpawnAreaCollision = EnemiesSpawnArea.GetChild<CollisionShape2D>(0);
        EnemiesSpawnAreaCollisionShape = EnemiesSpawnArea.GetChild<CollisionShape2D>(0).Shape as RectangleShape2D;
        Player = GetNode<Player>("Level/Player");
        LevelStartPlayerStopPoint = GetNode<Node2D>("Level/LevelStartPlayerStopPoint");
        MessageSprite = GetNode<Node2D>("HUD/Message");
        MessageGoSprite = GetNode<Node2D>("HUD/Message/MessageGo");
        MessageReadySprite = GetNode<Node2D>("HUD/Message/MessageReady");
        MessageLevelCompleteSprite = GetNode<Node2D>("HUD/Message/MessageLevelComplete");

        // Init game
        score = 0;
        enemiesLeft = enemiesInitialNumber;
        nextEnemySpawnTime = (ulong)random.Next(enemySpawnTimeMin, enemySpawnTimeMax) + Time.GetTicksMsec();

        // If windowed by default - expand window until it large enough to fit the screen
        if (GetViewport().GetWindow().Mode == Window.ModeEnum.Windowed)
        {
            Vector2I windowSize = GetViewport().GetWindow().Size;
            Vector2I screenSize = DisplayServer.ScreenGetSize();

            while (windowSize.X < screenSize.X || windowSize.Y < screenSize.Y)
            {
                GetViewport().GetWindow().Size = windowSize;
                windowSize += windowSize;
            }

            GetViewport().GetWindow().MoveToCenter();
        }

        // Create image and assign to BloodDrawSprite
        Image bloodDrawSpriteImage = Image.Create(
            (int)GetViewportRect().Size.X,
            (int)GetViewportRect().Size.Y,
            false,
            Image.Format.Rgba8
        );

        GetNode<Sprite2D>("Level/BloodDrawSprite").Texture = ImageTexture.CreateFromImage(bloodDrawSpriteImage);
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
    {
        HandlePauseToggle();
        HandleMusic();
        HandleFullscreenToggle();
        destroyOutOfBoundsProjectiles();
        HandleEnemiesSpawn();
        
        GD.Print("Level intro status: " + levelIntroStatus.ToString());
        HandleLevelIntro();
    }

    private void HandleLevelIntro()
    {
        if (levelIntroStatus == LevelIntroStatus.RUNNING_UP)
        {
            if (Math.Abs(Player.GlobalPosition.DistanceTo(LevelStartPlayerStopPoint.GlobalPosition)) < 1)
            {
                levelIntroStatus = LevelIntroStatus.STANDING;
            }
            else
            {
                Player.SetDirection(LevelStartPlayerStopPoint.GlobalPosition - Player.GlobalPosition);
            }
        }
    }

    private void HandleEnemiesSpawn()
    {
        if (!isGamePaused)
        {
            if (enemiesLeft > 0)
            {
                if (Time.GetTicksMsec() > nextEnemySpawnTime)
                {
                    enemiesLeft--;
                    nextEnemySpawnTime = (ulong)random.Next(enemySpawnTimeMin, enemySpawnTimeMax) + Time.GetTicksMsec();

                    int xLeft = (int)(EnemiesSpawnAreaCollision.GlobalPosition.X -
                                      EnemiesSpawnAreaCollisionShape.Size.X / 2);
                    int xRight = (int)(EnemiesSpawnAreaCollision.GlobalPosition.X +
                                       EnemiesSpawnAreaCollisionShape.Size.X / 2);
                    int xFinal = random.Next(xLeft, xRight);

                    Vector2 spawnPoint = new Vector2(
                        xFinal,
                        EnemiesSpawnAreaCollision.GlobalPosition.Y + EnemiesSpawnAreaCollisionShape.Size.Y
                    );

                    var enemy = GD.Load<PackedScene>("res://GameObjects/Characters/Enemy.tscn").Instantiate<Enemy>();

                    enemy.Type = random.Next(100) < GrenadeEnemySpawnChance
                        ? Enemy.EnemyType.GRENADE
                        : Enemy.EnemyType.PISTOL;

                    enemy.GlobalPosition = spawnPoint;
                    GetNode("Level/Enemies").AddChild(enemy);
                }
            }
        }
    }

    private void HandleMusic()
    {
        if (!isGamePaused)
        {
            GameplayPlayer.VolumeDb = 0;
            MenuPlayer.VolumeDb = -80;
        }
        else
        {
            MenuPlayer.VolumeDb = 0;
            GameplayPlayer.VolumeDb = -80;
        }
    }

    private void destroyOutOfBoundsProjectiles()
    {
        foreach (Node2D projectile in ProjectilesContainer.GetChildren())
        {
            bool xValid = projectile.GlobalPosition.X > 0 && projectile.GlobalPosition.X < GetViewportRect().Size.X;
            bool yValid = projectile.GlobalPosition.Y > 0 && projectile.GlobalPosition.Y < GetViewportRect().Size.Y;

            if (!xValid || !yValid)
                projectile.QueueFree();
        }
    }

    private void HandlePauseToggle()
    {
        if (Input.IsActionJustPressed("pause_toggle") && isGameStarted)
        {
            isGamePaused = !isGamePaused;
            Menu.Visible = isGamePaused;
            Level.Visible = !isGamePaused;
            HUD.Visible = !isGamePaused;
        }
    }

    public void Exit()
    {
        GetTree().Quit();
    }

    public void StartGame()
    {
        if (!isGameStarted)
        {
            levelIntroStatus = LevelIntroStatus.RUNNING_UP;
            GetNode<Timer>("Level/LevelStartBeforeMessagesTimer").Start();
            Player.GlobalPosition = GetNode<Node2D>("Level/LevelStartPlayerPoint").GlobalPosition;
            
            TextureButton startButton = GetNode<TextureButton>("Menu/MenuOptions/StartGame");
            Texture2D continueButtonNormal = GD.Load<Texture2D>("res://Assets/Menu/OptionContinue.png");
            Texture2D continueButtonPressed = GD.Load<Texture2D>("res://Assets/Menu/OptionContinueSelected.png");
            startButton.TextureNormal = continueButtonNormal;
            startButton.TexturePressed = continueButtonPressed;
            startButton.TextureFocused = continueButtonPressed;
            startButton.TextureHover = continueButtonPressed;

            GameplayPlayer.Stream = LevelStartMusic;
            GameplayPlayer.Play();
        }

        isGameStarted = true;
        isGamePaused = false;
        Menu.Hide();
        Level.Show();
        HUD.Show();
    }

    private void HandleFullscreenToggle()
    {
        if (Input.IsActionJustPressed("toggle_fullscreen"))
        {
            GetViewport().GetWindow().Mode = GetViewport().GetWindow().Mode == Window.ModeEnum.ExclusiveFullscreen
                ? Window.ModeEnum.Windowed
                : Window.ModeEnum.ExclusiveFullscreen;
        }
    }

    private void OnGameplayMusicFinished()
    {
        if (GameplayPlayer.Stream == LevelStartMusic)
        {
            GameplayPlayer.Stream = LevelMusic;
            GameplayPlayer.Play();
        }
    }

    private void OnEnemyEntersSpawnArea(Node2D node)
    {
        if (node is Enemy enemy)
        {
            if (enemy.IsEmpty())
            {
                enemy.QueueFree();
            }
        }
    }
    
    private void ShowReadyMessage()
    {
        levelIntroStatus = LevelIntroStatus.MESSAGE_READY;
        MessageSprite.Show();
        MessageReadySprite.Show();
        GetNode<Timer>("Level/LevelStartMessagesTimer").Start();
    }

    private void CycleIntroMessages()
    {
        if (levelIntroStatus == LevelIntroStatus.MESSAGE_READY)
        {
            GetNode<Timer>("Level/LevelStartMessagesTimer").Start();
            
            if (MessageReadySprite.Visible)
            {
                MessageReadySprite.Hide();
            }
            else
            {
                levelIntroStatus = LevelIntroStatus.MESSAGE_GO;
                MessageGoSprite.Show();
            }
        }
        else
        if (levelIntroStatus == LevelIntroStatus.MESSAGE_GO)
        {
            if (MessageGoSprite.Visible)
            {
                GetNode<Timer>("Level/LevelStartMessagesTimer").Start();
                MessageGoSprite.Hide();
                MessageSprite.Hide();
            }
            else
            {
                levelIntroStatus = LevelIntroStatus.LEVEL_STARTED;
            }
        }
    }
}