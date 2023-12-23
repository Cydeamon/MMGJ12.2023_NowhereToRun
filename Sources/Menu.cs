using Godot;
using System;
using Godot.Collections;

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

        // Submenus "Go Back" options
        foreach (Control submenu in GetNode("Submenus").GetChildren())
            if (submenu.Name != "Backdrop")
                submenu.GetNode<TextureButton>("GoBack").Pressed += GoBack;
    }

    public override void _Process(double delta)
    {
        base._Process(delta);
        
        if (ActiveSubmenu != null)
        {
            if (Input.IsActionJustPressed("ui_cancel"))
                GoBack();
        }
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
    }
}