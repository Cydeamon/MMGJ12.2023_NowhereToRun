using System;
using System.IO;
using System.Text.RegularExpressions;
using Godot;
using Godot.Collections;
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

    public enum LevelOutroStatus
    {
        LEVEL_IS_RUNNING,
        FINISHED,
        WALL_DESTROYED,
        TRANSITION_TO_NEXT_LEVEL
    }

    private static LevelIntroStatus levelIntroStatus;
    private static LevelOutroStatus levelOutroStatus = LevelOutroStatus.LEVEL_IS_RUNNING;
    private static bool isGameStarted = false;
    private static bool isGamePaused = true;
    private int score = 0;
    private int enemiesInitialNumber = 10;
    private int enemiesLeft = 10;
    private int enemiesKilled = 0;
    private int level = 1;
    [Export] private int enemySpawnTimeMin = 250;
    [Export] private int enemySpawnTimeMax = 1500;
    [Export] public float GrenadeEnemySpawnChance = 10;
    private ulong nextEnemySpawnTime;
    protected readonly Random random = new Random();
    private AudioStream LevelStartMusic = GD.Load<AudioStream>("res://Assets/Music/LevelStart.wav");
    private AudioStream LevelMusic = GD.Load<AudioStream>("res://Assets/Music/Gameplay.wav");
    private double Delta;

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
    public Node2D MessageDeadSprite;
    public Label StatusLabel;


    /***************************************************************************/
    /***************************** Global game data ****************************/

    public static bool IsGameStarted() => isGameStarted;
    public static bool IsGamePaused() => isGamePaused;
    public static LevelIntroStatus GetLevelIntroStatus() => levelIntroStatus;

    /***************************************************************************/
    /********************************* Methods *********************************/

    public override void _Ready()
    {
        // Initialize nodes
        Menu = GetNode<Control>("Menu");
        Level = GetNode<Node2D>("Level");
        HUD = GetNode<Node2D>("HUD");
        ProjectilesContainer = GetNode("Level/Projectiles");
        MenuPlayer = GetNode<AudioStreamPlayer>("MenuPlayer");
        GameplayPlayer = GetNode<AudioStreamPlayer>("GameplayPlayer");
        EnemiesSpawnArea = GetNode<Area2D>("Level/EnemiesSpawnArea");
        EnemiesSpawnAreaCollision = EnemiesSpawnArea.GetChild<CollisionShape2D>(0);
        EnemiesSpawnAreaCollisionShape = EnemiesSpawnAreaCollision.Shape as RectangleShape2D;
        Player = GetNode<Player>("Level/Player");
        LevelStartPlayerStopPoint = GetNode<Node2D>("Level/LevelStartPlayerStopPoint");
        MessageSprite = GetNode<Node2D>("HUD/Message");
        MessageGoSprite = GetNode<Node2D>("HUD/Message/MessageGo");
        MessageReadySprite = GetNode<Node2D>("HUD/Message/MessageReady");
        MessageLevelCompleteSprite = GetNode<Node2D>("HUD/Message/MessageLevelComplete");
        MessageDeadSprite = GetNode<Node2D>("HUD/Message/MessageDead");
        StatusLabel = GetNode<Label>("HUD/Status");

        // Initialize game
        score = 0;
        enemiesLeft = enemiesInitialNumber;
        nextEnemySpawnTime = (ulong)random.Next(enemySpawnTimeMin, enemySpawnTimeMax) + Time.GetTicksMsec();

        Player.Killed += OnPlayerKilled;

        // Expand window if windowed by default
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

        // Create image for BloodDrawSprite
        Image bloodDrawSpriteImage = Image.Create(
            (int)GetViewportRect().Size.X,
            (int)GetViewportRect().Size.Y,
            false,
            Image.Format.Rgba8
        );

        GetNode<Sprite2D>("Level/BloodDrawSprite").Texture = ImageTexture.CreateFromImage(bloodDrawSpriteImage);

        // Initialize settings
        ReadINI();
    }

    private void ReadINI()
    {
        string filePath = "NowhereToRun.ini";
        using StreamReader reader = new StreamReader(filePath);

        string sectionNamePattern = @"\[([A-Z_]+)\]";
        string sectionParamPattern = @"([A-Z_]+)=([0-9]+)";

        string line = "", currentSection = "", paramName = "", paramValue = "";
        while ((line = reader.ReadLine()) != null)
        {
            ///////////////////////
            // Read string

            // Skip empty lines
            if (line.Trim() == "")
                continue;

            // Skip comments
            if (line.StartsWith(";"))
                continue;

            if (Regex.IsMatch(line, sectionNamePattern))
            {
                currentSection = Regex.Match(line, sectionNamePattern).Groups[1].Value;
                continue;
            }
            else if (Regex.IsMatch(line, sectionParamPattern))
            {
                paramName = Regex.Match(line, sectionParamPattern).Groups[1].Value;
                paramValue = Regex.Match(line, sectionParamPattern).Groups[2].Value;

                if (paramValue.Trim() == "" || paramName.Trim() == "")
                {
                    GD.PrintErr("Invalid INI line: " + line + " in section '" + currentSection + "'. Skipping.");
                    continue;
                }
            }
            else
            {
                GD.PrintErr("Invalid INI line: " + line + " in section '" + currentSection + "'. Skipping.");
                continue;
            }

            ///////////////////////
            // Process string

            if (currentSection == "HIGHSCORES")
            {
                RichTextLabel scores = GetNode<RichTextLabel>("Menu/Submenus/HighScores/HBoxContainer/Scores");
                RichTextLabel names = GetNode<RichTextLabel>("Menu/Submenus/HighScores/HBoxContainer/Names");

                names.AddText("\n" + paramName);
                scores.AddText("\n" + paramValue);
            }
        }
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
    {
        Delta = delta;

        HandlePauseToggle();
        HandleMusic();
        HandleFullscreenToggle();
        destroyOutOfBoundsProjectiles();
        HandleEnemiesSpawn();
        HandleLevelIntro();

        if (!Player.IsDead())
        {
            StatusLabel.Text =
                $"Level: {level}    Score: {score}    Enemies remaining: {enemiesInitialNumber - enemiesKilled}";
        }
    }

    private void HandleLevelIntro()
    {
        if (levelIntroStatus == LevelIntroStatus.RUNNING_UP)
        {
            if (Math.Abs(Player.GlobalPosition.DistanceTo(LevelStartPlayerStopPoint.GlobalPosition)) < 1)
            {
                levelIntroStatus = LevelIntroStatus.STANDING;
                Player.SetDirection(Vector2.Zero);
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
                    nextEnemySpawnTime = (ulong)random.Next(
                        levelIntroStatus != LevelIntroStatus.LEVEL_STARTED ? 1000 : enemySpawnTimeMin,
                        levelIntroStatus != LevelIntroStatus.LEVEL_STARTED ? 2000 : enemySpawnTimeMax
                    ) + Time.GetTicksMsec();

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
                    enemy.Killed += OnEnemyKilled;
                    GetNode("Level/Enemies").AddChild(enemy);
                }
            }
        }
    }

    private void HandleMusic()
    {
        if (!isGamePaused)
        {
            if (Player.IsDead())
            {
                GameplayPlayer.PitchScale = (float)Mathf.Lerp(GameplayPlayer.PitchScale, 0.5f, 2f * Delta);
            }
            else
            {
                GameplayPlayer.PitchScale = 1f;
                GameplayPlayer.VolumeDb = 0;
                MenuPlayer.VolumeDb = -80;
            }
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

            if (Player.IsDead())
            {
                GameReset();
            }
        }
    }

    private void GameReset(bool resetScore = true)
    {
        if (resetScore)
        {
            score = 0;
            enemiesInitialNumber = 10;
            enemiesLeft = 10;
            enemiesKilled = 0;
            level = 1;
        }

        isGameStarted = false;
        isGamePaused = true;

        foreach (Node2D enemy in GetNode<Node2D>("Level/Enemies").GetChildren())
            enemy.QueueFree();

        Image bloodDrawSpriteImage = Image.Create(
            (int)GetViewportRect().Size.X,
            (int)GetViewportRect().Size.Y,
            false,
            Image.Format.Rgba8
        );

        GetNode<Sprite2D>("Level/BloodDrawSprite").Texture = ImageTexture.CreateFromImage(bloodDrawSpriteImage);

        TextureButton startButton = GetNode<TextureButton>("Menu/MenuOptions/StartGame");
        Texture2D startGameButtonNormal = GD.Load<Texture2D>("res://Assets/Menu/OptionStartGame.png");
        Texture2D startGameButtonPressed = GD.Load<Texture2D>("res://Assets/Menu/OptionStartGameSelected.png");
        startButton.TextureNormal = startGameButtonNormal;
        startButton.TexturePressed = startGameButtonPressed;
        startButton.TextureFocused = startGameButtonPressed;
        startButton.TextureHover = startGameButtonPressed;

        levelOutroStatus = LevelOutroStatus.LEVEL_IS_RUNNING;
        levelIntroStatus = LevelIntroStatus.RUNNING_UP;

        Player.Reset();
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

            Player.StopBleeding();
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
                enemiesLeft--;
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
        else if (levelIntroStatus == LevelIntroStatus.MESSAGE_GO)
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

    private void OnPlayerKilled()
    {
        // Show game over message
        StatusLabel.Text = "Game over. Press ESC to return to main menu.";
        MessageSprite.Show();
        MessageDeadSprite.Show();

        // Check high scores
        RichTextLabel scoresLabel = GetNode<RichTextLabel>("Menu/Submenus/HighScores/HBoxContainer/Scores");
        string[] scores = scoresLabel.Text.Split("\n");
        bool newHighScore = false; 

        foreach (string tempScore in scores)
        {
            if (tempScore.ToInt() > score)
            {
                newHighScore = true;
                break;
            }
        }
        
        // If there's new high score - show message to enter name
        if (newHighScore)
        {
            GetNode<Node2D>("HUD/NewHighscoreBlock").Show();
        }
    }


    private void OnEnemyKilled()
    {
        score += 5 * level;
        enemiesKilled++;

        if (enemiesInitialNumber - enemiesKilled == 0)
        {
            MessageSprite.Show();
            MessageLevelCompleteSprite.Show();
            GetNode<Timer>("Level/EndLevelTimer").Start();
        }
    }

    private void OnEndLevelTimer()
    {
        if (levelOutroStatus == LevelOutroStatus.LEVEL_IS_RUNNING)
        {
            MessageSprite.Show();
            MessageLevelCompleteSprite.Show();
            levelOutroStatus = LevelOutroStatus.FINISHED;
            GetNode<Timer>("Level/EndLevelTimer").Start();
        }
        else if (levelOutroStatus == LevelOutroStatus.FINISHED)
        {
            MessageSprite.Hide();
            MessageLevelCompleteSprite.Hide();
            levelOutroStatus = LevelOutroStatus.TRANSITION_TO_NEXT_LEVEL;
            GetNode<Timer>("Level/EndLevelTimer").Start();
        }
        else if (levelOutroStatus == LevelOutroStatus.TRANSITION_TO_NEXT_LEVEL)
        {
            GameReset(false);
            level += 1;
            enemiesKilled = 0;
            enemiesInitialNumber = (int)((float)enemiesInitialNumber * 1.5);
            enemiesLeft = enemiesInitialNumber;
            MessageSprite.Hide();
            MessageLevelCompleteSprite.Hide();
            levelOutroStatus = LevelOutroStatus.LEVEL_IS_RUNNING;
            isGameStarted = true;
            isGamePaused = false;

            levelIntroStatus = LevelIntroStatus.RUNNING_UP;
            GetNode<Timer>("Level/LevelStartBeforeMessagesTimer").Start();
            Player.GlobalPosition = GetNode<Node2D>("Level/LevelStartPlayerPoint").GlobalPosition;
            Player.SetDirection(Vector2.Up);

            GameplayPlayer.Stream = LevelStartMusic;
            GameplayPlayer.Play();
        }
    }
}