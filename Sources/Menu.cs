using Godot;
using System;

public partial class Menu : Control
{
    /***************************************************************************/
    /********************************* Signals *********************************/
    [Signal] public delegate void StartGameEventHandler();

    [Signal] public delegate void HighScoresEventHandler();

    [Signal] public delegate void CreditsEventHandler();

    [Signal] public delegate void ExitEventHandler();


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

        // Submenus "Go Back" options
        foreach (Control submenu in GetNode("Submenus").GetChildren())
            submenu.GetNode<TextureButton>("GoBack").Pressed += GoBack;
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
        }
    }

    public void GoBack()
    {
        ActiveSubmenu.Hide();
        ActiveSubmenu = null;
        GetNode<Control>("MenuOptions").Show();
        GetNode<Node2D>("InfoLabel").Show();
    }

    public void OnStartGamePressed()
    {
        EmitSignal("StartGame");
    }

    public void OnExitPressed()
    {
        EmitSignal("Exit");
    }
}