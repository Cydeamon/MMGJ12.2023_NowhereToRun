using Godot;
using System;
using Godot.Collections;
using static NowhereToRun.Sources.Main;

public partial class Menu : Control
{
    /***************************************************************************/
    /********************************* Signals *********************************/
    [Signal]
    public delegate void StartGameEventHandler();

    [Signal]
    public delegate void HighScoresEventHandler();

    [Signal]
    public delegate void CreditsEventHandler();

    [Signal]
    public delegate void ExitEventHandler();


    /***************************************************************************/
    /******************************** Properties *******************************/

    private Control ActiveSubmenu = null;


    /***************************************************************************/
    /********************************* Methods *********************************/


    public override void _Ready()
    {
        // Connect signals
        GetNode<TextureButton>("MenuOptions/StartGame").Pressed += OnStartGamePressed;
        GetNode<TextureButton>("MenuOptions/Exit").Pressed += OnExitPressed;

        // Submenus activation
        foreach (TextureButton option in GetNode<VBoxContainer>("MenuOptions").GetChildren())
        {
            if (option.Name != "StartGame" && option.Name != "Exit")
                option.Pressed += ActivateSubmenu;
        }

        // GoBack on "Go back" button press in submenus
        foreach (Node submenu in GetNode("Submenus").GetChildren())
        {
            TextureButton goBackButton = submenu.GetNode<TextureButton>("GoBack");

            if (goBackButton != null)
                goBackButton.Pressed += GoBack;
        }

        // Options neighbours order
        Array<Node> options = GetNode<VBoxContainer>("MenuOptions").GetChildren();
        for (int i = 0; i < options.Count; i++)
        {
            TextureButton option = options[i] as TextureButton;

            if (i != 0)
            {
                TextureButton previousOption = options[i - 1] as TextureButton;
                option.FocusNeighborTop = previousOption.GetPath();
            }

            if (i != options.Count - 1)
            {
                TextureButton nextOption = options[i + 1] as TextureButton;
                option.FocusNeighborBottom = nextOption.GetPath();
            }
        }

        // Options in "Options" menu neighbours order
        options = GetNode<Control>("Submenus/Options/GridContainer").GetChildren();

        for (int i = 0; i < options.Count; i++)
        {
            if (i % 2.0f == 0)
            {
                Button option = options[i] as Button;

                if (i != 0)
                {
                    Button previousOption = options[i - 2] as Button;
                    option.FocusNeighborTop = previousOption.GetPath();
                }

                if (i != options.Count - 2)
                {
                    Button nextOption = options[i + 2] as Button;
                    option.FocusNeighborBottom = nextOption.GetPath();
                }
            }
        }
    }

    public override void _Process(double delta)
    {
        base._Process(delta);

        if (ActiveSubmenu != null)
        {
            if (Input.IsActionJustPressed("ui_cancel"))
                GoBack();
        }

        if (Input.IsActionJustPressed("ui_up") || Input.IsActionJustPressed("ui_down"))
        {
            if (GetViewport().GuiGetFocusOwner() == null && ActiveSubmenu == null)
            {
                GetNode<TextureButton>("MenuOptions/StartGame").GrabFocus();
            }
            
            if (ActiveSubmenu != null)
            {
                if (IsControllerMode())
                {
                    if (ActiveSubmenu.Name != "Options")
                        GetNode<TextureButton>("Submenus/" + ActiveSubmenu.Name + "/GoBack").GrabFocus();
                }
            }
        }


        // Handle options values change in "Options" menu
        if (ActiveSubmenu?.Name == "Options")
        {
            if (Input.IsActionJustPressed("ui_left") || Input.IsActionJustPressed("ui_right"))
            {
                if (GetViewport().GuiGetFocusOwner() is Button option)
                {
                    Control control =
                        GetNode<Control>(
                            "Submenus/Options/GridContainer/" + option.Name.ToString().Replace("Label", ""));

                    if (control is ProgressBar progressBar)
                    {
                        if (Input.IsActionJustPressed("ui_left"))
                            progressBar.Value -= progressBar.Step;
                        else
                            progressBar.Value += progressBar.Step;

                        OnVolumeSliderValueChanged();
                    }

                    option.GrabFocus();
                }
            }
        }
    }

    public void UpdateSettings()
    {
        GetNode<ProgressBar>("Submenus/Options/GridContainer/MusicVolume").Value = GlobalGameState.MusicVolume;
        GetNode<ProgressBar>("Submenus/Options/GridContainer/SoundsVolume").Value = GlobalGameState.SoundsVolume;
    }

    public void ActivateSubmenu()
    {
        if (GetViewport().GuiGetFocusOwner() is TextureButton menuOption)
        {
            Control submenu = GetNode<Control>("Submenus/" + menuOption.Name);
            ActiveSubmenu = submenu;
            submenu.Show();
            GetNode<Control>("MenuOptions").Hide();
            GetNode<Node2D>("InfoLabel").Hide();
            GetNode<Node2D>("Logo").Hide();
            GetNode<ColorRect>("Submenus/Backdrop").Show();

            if (menuOption.Name == "Options")
            {
                Button firstOption =
                    GetNode<GridContainer>("Submenus/Options/GridContainer").GetChildren()[0] as Button;
                firstOption.GrabFocus();
            }
        }
    }

    public void GoBack()
    {
        ActiveSubmenu.Hide();
        ActiveSubmenu = null;
        GetNode<Control>("MenuOptions").Show();
        GetNode<Node2D>("InfoLabel").Show();
        GetNode<Node2D>("Logo").Show();
        GetNode<ColorRect>("Submenus/Backdrop").Hide();
    }

    public void OnStartGamePressed()
    {
        EmitSignal("StartGame");
    }

    public void OnExitPressed()
    {
        EmitSignal("Exit");
    }

    public void ActivateSubmenu(string name)
    {
        TextureButton menuOption = GetNode<TextureButton>("MenuOptions/" + name);
        Control submenu = GetNode<Control>("Submenus/" + menuOption.Name);
        ActiveSubmenu = submenu;
        submenu.Show();
        GetNode<Control>("MenuOptions").Hide();
        GetNode<Node2D>("InfoLabel").Hide();

        if (menuOption.Name == "Options")
        {
            Button firstOption = GetNode<GridContainer>("Submenus/Options/GridContainer").GetChildren()[0] as Button;
            firstOption.GrabFocus();
        }
    }

    public void ToggleFullscreen()
    {
        GetViewport().GetWindow().Mode = GetViewport().GetWindow().Mode == Window.ModeEnum.ExclusiveFullscreen
            ? Window.ModeEnum.Windowed
            : Window.ModeEnum.ExclusiveFullscreen;

        GlobalGameState.Fullscreen = GetViewport().GetWindow().Mode == Window.ModeEnum.ExclusiveFullscreen;
    }

    private void OnVolumeSliderValueChanged()
    {
        GlobalGameState.MusicVolume = (int)GetNode<ProgressBar>("Submenus/Options/GridContainer/MusicVolume").Value;
        GlobalGameState.SoundsVolume = (int)GetNode<ProgressBar>("Submenus/Options/GridContainer/SoundsVolume").Value;
    }
}