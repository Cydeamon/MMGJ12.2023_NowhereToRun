using System;
using Godot;

namespace NowhereToRun.Sources;

public partial class Main : Node2D
{
    /***************************************************************************/
    /***************************** Game properties *****************************/

    private static bool isGameStarted = false;
    private static bool isGamePaused = true;
    private int score = 0;

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


    /***************************************************************************/
    /***************************** Global game data ****************************/

    public static bool IsGameStarted() => isGameStarted;
    public static bool IsGamePaused() => isGamePaused;

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
        GD.Print("FINISHED");
        if (GameplayPlayer.Stream == LevelStartMusic)
        {
            GameplayPlayer.Stream = LevelMusic;
            GameplayPlayer.Play();
        }
    }
}