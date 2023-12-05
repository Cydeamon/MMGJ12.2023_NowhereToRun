using System;
using Godot;

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

    /****************************************************************************/
    /******************************** Methods ***********************************/

    public override void _Process(double delta)
    {
        base._Process(delta);
        pickAnimation();
    }

    protected override void pickAnimation()
    {
        string targetAnimation = "";
        
        if (targetAnimation != characterSprite.Animation)
            characterSprite.Play(targetAnimation);
    }
}