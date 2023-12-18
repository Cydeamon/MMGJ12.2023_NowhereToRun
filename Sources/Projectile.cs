using Godot;
using System;
using System.Collections.Generic;
using System.Reflection;
using NowhereToRun.Sources.Characters;
using static NowhereToRun.Sources.Main;

public partial class Projectile : StaticBody2D
{
    /***************************************************************************/
    /********************************** Nodes **********************************/


    /***************************************************************************/
    /******************************* Properties ********************************/

    [Export] private float speed = 200f;
    [Export] public Vector2 Direction = Vector2.Zero;

    private double delta;

    /***************************************************************************/
    /******************************* Methods **********************************/

    public override void _Ready()
    {
        ConstantLinearVelocity = Direction.Normalized() * speed;
    }

    public override void _PhysicsProcess(double delta)
    {
        this.delta = delta;

        if (IsGameStarted() && !IsGamePaused())
        {
            advance();
        }
    }

    private void advance()
    {
        KinematicCollision2D collision = MoveAndCollide(ConstantLinearVelocity * (float)delta);

        if (collision != null)
        {
            Character character = collision.GetCollider() as Character;

            if (character != null && !character.IsDead())
                character.BulletHit(Direction);

            QueueFree();
        }
    }
}