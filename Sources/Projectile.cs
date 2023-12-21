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
    public Character Shooter;
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
        foreach (Node2D body in bodies)
        {
            if (body is Character)
                continue;

            QueueFree();
        }

        // Kill target on hitting target's HitArea
        foreach (Area2D area in areas)
        {
            Character character = area.GetParent() as Character;

            if (area.Name != "HitArea")
                continue;
            
            if (character != null && !character.IsDead() && Shooter != character)
            {
                character.BulletHit(Direction);
                QueueFree();
            }
        }
    }
}