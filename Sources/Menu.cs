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
    /********************************* Methods *********************************/


    public override void _Ready()
    {
        // Connect signals
        GetNode<TextureButton>("MenuOptions/StartGame").Pressed += OnStartGamePressed;
        GetNode<TextureButton>("MenuOptions/HighScores").Pressed += OnHighScoresPressed;
        GetNode<TextureButton>("MenuOptions/Credits").Pressed += OnCreditsPressed;
        GetNode<TextureButton>("MenuOptions/Exit").Pressed += OnExitPressed;
    }


    public void OnStartGamePressed()
    {
        EmitSignal("StartGame");
    }

    public void OnHighScoresPressed()
    {
        EmitSignal("HighScores");
    }

    public void OnCreditsPressed()
    {
        EmitSignal("Credits");
    }

    public void OnExitPressed()
    {
        EmitSignal("Exit");
    }
}