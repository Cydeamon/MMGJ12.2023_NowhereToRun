using Godot;

namespace NowhereToRun.Sources;

public partial class Main : Node2D
{
    /***************************************************************************/
    /***************************** Game properties *****************************/

    private static bool isGameStarted = false;
    private static bool isGamePaused = false;
    private int score = 0;

    /***************************************************************************/
    /********************************** Nodes **********************************/

    public Control Menu;
    public Node2D Level;
    public Node2D HUD;
    
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
        
        // If windowed by default - expand window until it large enough to fit the screen
        if (GetViewport().GetWindow().Mode == Window.ModeEnum.Windowed)
        {
            Vector2I windowSize = GetViewport().GetWindow().Size;
            Vector2I screenSize = DisplayServer.ScreenGetSize();
            
            while(windowSize.X < screenSize.X || windowSize.Y < screenSize.Y)
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
        HandleFullscreenToggle();
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
        }
        
        isGameStarted = true;
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
}