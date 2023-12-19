using Godot;
using System;
using System.Collections.Generic;
using System.Reflection;
using Godot.Collections;
using NowhereToRun.Sources.Characters;
using static NowhereToRun.Sources.Main;

public partial class Projectile : Area2D
{
    /***************************************************************************/
    /********************************** Nodes **********************************/


    /***************************************************************************/
    /******************************* Properties ********************************/

    [Export] private float speed = 200f;
    [Export] public Vector2 Direction = Vector2.Zero;
    [Export] public bool HurtPlayer = true;
    private double delta;
    private Vector2 velocity;

    /***************************************************************************/
    /******************************* Methods **********************************/

    public override void _Ready()
    {
        velocity = Direction.Normalized() * speed;
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
        Position += velocity * (float)delta;
        Array<Node2D> bodies = GetOverlappingBodies();
        Array<Area2D> areas = GetOverlappingAreas();

        // Destroy bullet on hitting wall
        for (int i = 0; i < bodies.Count; i++)
        {
            if (bodies[i] is Character)
                continue;

            QueueFree();
        }

        // Kill target on hitting target's HitArea
        for (int i = 0; i < areas.Count; i++)
        {
            Area2D area = areas[i];

            if (area.Name != "HitArea")
                continue;

            Character character = area.GetParent() as Character;
            bool isPlayer = character is Player;
            bool isEnemy = character is Enemy;

            if ((character != null && !character.IsDead()) &&
                ((isPlayer && HurtPlayer) || isEnemy))
            {
                character.BulletHit(Direction);
                QueueFree();
            }
        }
    }
}