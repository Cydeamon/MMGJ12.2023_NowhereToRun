using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
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

    public enum LevelOutroStatus
    {
        LEVEL_IS_RUNNING,
        FINISHED,
        WALL_DESTROYED,
        TRANSITION_TO_NEXT_LEVEL
    }

    private struct HighScoreEntry
    {
        public string Name;
        public int Score;

        public HighScoreEntry(string name, string score)
        {
            Name = name;
            Score = score.ToInt();
        }

        public HighScoreEntry(string name, int score)
        {
            Name = name;
            Score = score;
        }
    }

    private static LevelIntroStatus levelIntroStatus;
    private static LevelOutroStatus levelOutroStatus = LevelOutroStatus.LEVEL_IS_RUNNING;
    private static bool isGameStarted = false;
    private static bool isGamePaused = true;
    private static bool isControllerMode = false;
    private bool highScoreNameInput = false;
    private int highScoreNameLetter = 0;
    private string highScoreName = "";
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
    private AudioStream LevelMusic1 = GD.Load<AudioStream>("res://Assets/Music/Gameplay1.wav");
    private AudioStream LevelMusic2 = GD.Load<AudioStream>("res://Assets/Music/Gameplay2.wav");
    private AudioStream ReadySound = GD.Load<AudioStream>("res://Assets/Sounds/Ready.wav");
    private AudioStream DeadSound = GD.Load<AudioStream>("res://Assets/Sounds/Dead.wav");
    private AudioStream GoSound = GD.Load<AudioStream>("res://Assets/Sounds/GO.wav");
    private static AudioStream ShootSound = GD.Load<AudioStream>("res://Assets/Sounds/Shoot.wav");
    private double Delta;
    private List<HighScoreEntry> highScores;

    /***************************************************************************/
    /********************************** Nodes **********************************/

    public Menu Menu;
    public Node2D Level;
    public Node2D HUD;
    public Node ProjectilesContainer;
    public AudioStreamPlayer MenuPlayer;
    public AudioStreamPlayer GameplayPlayer;
    public AudioStreamPlayer MessagesPlayer;
    public AudioStreamPlayer SoundsPlayer;
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
    private Vector2 lastMousePosition;


    /***************************************************************************/
    /***************************** Global game data ****************************/

    public static bool IsGameStarted() => isGameStarted;
    public static bool IsGamePaused() => isGamePaused;
    public static bool IsControllerMode() => isControllerMode;
    public static LevelIntroStatus GetLevelIntroStatus() => levelIntroStatus;
    

    /***************************************************************************/
    /********************************* Methods *********************************/

    public override void _Ready()
    {
        // Initialize nodes
        Menu = GetNode<Menu>("Menu");
        Level = GetNode<Node2D>("Level");
        HUD = GetNode<Node2D>("HUD");
        ProjectilesContainer = GetNode("Level/Projectiles");
        MenuPlayer = GetNode<AudioStreamPlayer>("MenuPlayer");
        GameplayPlayer = GetNode<AudioStreamPlayer>("GameplayPlayer");
        MessagesPlayer = GetNode<AudioStreamPlayer>("MessagesPlayer");
        SoundsPlayer = GetNode<AudioStreamPlayer>("SoundsPlayer");
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
        
        // Initialize variables
        highScores = new List<HighScoreEntry>();
        
        // Read settings
        ReadINI();
        Menu.UpdateSettings();

        // Initialize game
        score = 0;
        enemiesLeft = enemiesInitialNumber;
        nextEnemySpawnTime = (ulong)random.Next(enemySpawnTimeMin, enemySpawnTimeMax) + Time.GetTicksMsec();

        Player.Killed += OnPlayerKilled;
        Player.PlayerShot += PlayShootSound;

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

        GetViewport().GetWindow().Mode = 
            GlobalGameState.Fullscreen ? Window.ModeEnum.ExclusiveFullscreen : Window.ModeEnum.Windowed;

        // Create image for BloodDrawSprite
        Image bloodDrawSpriteImage = Image.Create(
            (int)GetViewportRect().Size.X,
            (int)GetViewportRect().Size.Y,
            false,
            Image.Format.Rgba8
        );

        GetNode<Sprite2D>("Level/BloodDrawSprite").Texture = ImageTexture.CreateFromImage(bloodDrawSpriteImage);
    }
    public override void _Process(double delta)
    {
        Delta = delta;
        HandleMusic();
        checkControlMethod();
        
        if (!highScoreNameInput)
        {
            HandlePauseToggle();
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
        else
        {
            HandleHighScoreNameInput();
        }
    }

    public override void _ExitTree()
    {
        base._ExitTree();
        
        // Save settings
        WriteINI();
    }

    private void WriteINI()
    {
        string filePath = "NowhereToRun.ini";
        using StreamWriter writer = new StreamWriter(filePath);
        
        writer.WriteLine("[SETTINGS]");
        writer.WriteLine($"Fullscreen={GlobalGameState.Fullscreen}");
        writer.WriteLine($"MusicVolume={GlobalGameState.MusicVolume}");
        writer.WriteLine($"SoundsVolume={GlobalGameState.SoundsVolume}");
        writer.WriteLine();
        writer.WriteLine("[HIGHSCORES]");

        for (int i = 0; i < highScores.Count; i++)
            writer.WriteLine($"{highScores[i].Name}={highScores[i].Score}");
        
        writer.Close();
    }

    private void ReadINI()
    {
        string filePath = "NowhereToRun.ini";
        using StreamReader reader = new StreamReader(filePath);

        string sectionNamePattern = @"\[([A-Z_]+)\]";
        string sectionParamPattern = @"([A-z_]+)=([Tt]rue|[Ff]alse|[0-9-]+)";

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

                highScores.Add(new HighScoreEntry(paramName, paramValue));
            }

            if (currentSection == "SETTINGS")
            {
                switch (paramName)
                {
                    case "MusicVolume":  GlobalGameState.MusicVolume = int.Parse(paramValue); break;
                    case "SoundsVolume": GlobalGameState.SoundsVolume = int.Parse(paramValue); break;
                    case "Fullscreen":   GlobalGameState.Fullscreen = bool.Parse(paramValue); break;
                }
            }
        }
    }

    private void checkControlMethod()
    {
        if (Input.IsActionJustPressed("joy_action"))
        {
            isControllerMode = true;
            Input.MouseMode = Input.MouseModeEnum.Hidden;
        }
        
        if (Input.IsActionJustPressed("ui_up") || Input.IsActionJustPressed("ui_down") ||
            Input.IsActionJustPressed("ui_left") || Input.IsActionJustPressed("ui_right"))
        {
            Input.MouseMode = Input.MouseModeEnum.Hidden;
        }

        if (GetViewport().GetMousePosition() != lastMousePosition)
        {
            lastMousePosition = GetViewport().GetMousePosition();
            isControllerMode = false;
            Input.MouseMode = Input.MouseModeEnum.Visible;
        }
    }

    private void HandleHighScoreNameInput()
    {
        if (Input.IsActionJustPressed("ui_cancel"))
        {
            highScoreNameInput = false;
            GetNode<Node2D>("HUD/NewHighscoreBlock").Hide();
            GameReset();
            Menu.Show();
        }
        else
        {
            GetNode<LineEdit>("HUD/NewHighscoreBlock/LineEdit").GrabFocus();

            if (Input.IsActionJustPressed("ui_accept"))
            {
                string name = GetNode<LineEdit>("HUD/NewHighscoreBlock/LineEdit").Text;

                if (name.Length == 3)
                {
                    highScoreNameInput = false;
                    highScores.Add(new HighScoreEntry(name, score));
                    SortHighScores();
                    UpdateHighScoresTable();
                    highScoreNameInput = false;
                    GetNode<Node2D>("HUD/NewHighscoreBlock").Hide();
                    GameReset();
                    Menu.Show();
                    Menu.ActivateSubmenu("HighScores");
                }
            }
        }
    }

    private void UpdateHighScoresTable()
    {
        RichTextLabel names = GetNode<RichTextLabel>("Menu/Submenus/HighScores/HBoxContainer/Names");
        RichTextLabel scores = GetNode<RichTextLabel>("Menu/Submenus/HighScores/HBoxContainer/Scores");
        names.Text = "";
        scores.Text = "";

        for (int i = 0; i < highScores.Count; i++)
        {
            names.Text += highScores[i].Name;

            if (i != highScores.Count - 1)
                names.Text += "\n";
        }

        for (int i = 0; i < highScores.Count; i++)
        {
            scores.Text += highScores[i].Score;

            if (i != highScores.Count - 1)
                scores.Text += "\n";
        }
    }

    private void SortHighScores()
    {
        highScores.Sort((a, b) => b.Score.CompareTo(a.Score));

        if (highScores.Count > 8)
            highScores.RemoveRange(8, highScores.Count - 8);
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
                    enemy.EnemyRunAway += OnEnemyRunAway;
                    enemy.EnemyShot += PlayShootSound;
                    GetNode("Level/Enemies").AddChild(enemy);
                }
            }
        }
    }

    private void PlayShootSound()
    {
        SoundsPlayer.Stream = ShootSound;
        SoundsPlayer.VolumeDb = GlobalGameState.SoundsVolume;
        SoundsPlayer.Play();
    }

    private void OnEnemyRunAway()
    {
        enemiesKilled++;

        if (enemiesInitialNumber - enemiesKilled == 0)
        {
            MessageSprite.Show();
            MessageLevelCompleteSprite.Show();
            GetNode<Timer>("Level/EndLevelTimer").Start();
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
                GameplayPlayer.VolumeDb = GlobalGameState.MusicVolume;
                MenuPlayer.VolumeDb = -80;
            }
        }
        else
        {
            MenuPlayer.VolumeDb = GlobalGameState.MusicVolume;
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

        MessageSprite.Hide();
        foreach (Node2D node in MessageSprite.GetChildren())
            node.Hide();

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
            GameplayPlayer.VolumeDb = GlobalGameState.MusicVolume;
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
            
            GlobalGameState.Fullscreen = GetViewport().GetWindow().Mode == Window.ModeEnum.ExclusiveFullscreen;
        }
    }

    private void OnGameplayMusicFinished()
    {
        if (GameplayPlayer.Stream == LevelStartMusic)
        {
            GameplayPlayer.Stream = LevelMusic1;
            GameplayPlayer.Play();
        }
        else if (GameplayPlayer.Stream == LevelMusic1)
        {
            GameplayPlayer.Stream = LevelMusic2;
            GameplayPlayer.VolumeDb = GlobalGameState.MusicVolume;
            GameplayPlayer.Play();
        }
        else if (GameplayPlayer.Stream == LevelMusic2)
        {
            GameplayPlayer.Stream = LevelMusic1;
            GameplayPlayer.VolumeDb = GlobalGameState.MusicVolume;
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
                enemiesKilled++;
            }
        }
    }

    private void ShowReadyMessage()
    {
        levelIntroStatus = LevelIntroStatus.MESSAGE_READY;
        MessageSprite.Show();
        MessageReadySprite.Show();

        if (!isGamePaused)
        {
            MessagesPlayer.Stream = ReadySound;
            MessagesPlayer.VolumeDb = GlobalGameState.SoundsVolume;
            MessagesPlayer.Play();
        }

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


                if (!isGamePaused)
                {
                    MessagesPlayer.Stream = GoSound;
                    MessagesPlayer.VolumeDb = GlobalGameState.SoundsVolume;
                    MessagesPlayer.Play();
                }
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
        if (!isGamePaused)
        {
            MessagesPlayer.Stream = DeadSound;
            MessagesPlayer.VolumeDb = GlobalGameState.SoundsVolume;
            MessagesPlayer.Play();
        }
        
        // Cancel "Level complete"
        if (levelOutroStatus == LevelOutroStatus.FINISHED)
        {
            MessageSprite.Hide();
            MessageLevelCompleteSprite.Hide();
            GetNode<Timer>("Level/EndLevelTimer").Stop();
        }

        // Show game over message
        StatusLabel.Text = "Game over. Press ESC to return to main menu.";
        MessageSprite.Show();
        MessageDeadSprite.Show();

        // Check high scores
        bool newHighScore = false;

        foreach (HighScoreEntry entry in highScores)
        {
            if (entry.Score < score)
            {
                newHighScore = true;
                break;
            }
        }

        // If there's new high score - show message to enter name
        if (newHighScore)
        {
            GetNode<Label>("HUD/NewHighscoreBlock/Score").Text = $"{score} - NEW HIGH SCORE!";
            GetNode<Node2D>("HUD/NewHighscoreBlock").Show();
            GetNode<LineEdit>("HUD/NewHighscoreBlock/LineEdit").GrabFocus();
            highScoreNameInput = true;
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
        if (!Player.IsDead())
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
                GameplayPlayer.VolumeDb = GlobalGameState.MusicVolume;
                GameplayPlayer.Play();
            }
        }
    }
}