using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public partial class Enemy : Character
{
    /****************************************************************************/
    /********************************** Enums ***********************************/

    [Flags]
    public enum EnemyType
    {
        PISTOL = 1 << 1,
        GRENADE = 1 << 2
    }

    /****************************************************************************/
    /******************************* Properties *********************************/

    [Export] public EnemyType Type = EnemyType.PISTOL;
    [Export] private bool isEmpty = false;

    /****************************************************************************/
    /******************************** Methods ***********************************/

    public override void _Process(double delta)
    {
        base._Process(delta);
        pickAnimation();
    }

    protected override void pickAnimation()
    {
        if (IsGameStarted() && !IsGamePaused() && !isDead)
        {
            string targetAnimation = "";

            if (isEmpty)
                targetAnimation = "Empty";
            else
            {
                targetAnimation = Type switch
                {
                    EnemyType.PISTOL  => "Pistol",
                    EnemyType.GRENADE => "Grenade",
                    _                 => "Empty"
                };
            }
            
            if (velocity.X > 0)
                targetAnimation += "RunRight";
            else if (velocity.X < 0)
                targetAnimation += "RunLeft";
            else if (velocity.Y != 0)
                targetAnimation += "RunVertical";

            if (velocity == Vector2.Zero)
                targetAnimation += "Idle";

            if (targetAnimation != characterSprite.Animation)
                characterSprite.Play(targetAnimation);
        }
    }
}