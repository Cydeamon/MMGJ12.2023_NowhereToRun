using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public partial class Player : Character
{
    /****************************************************************************/
    /******************************** Signals ***********************************/

    [Signal] public delegate void PlayerShotEventHandler();
    
    /****************************************************************************/
    /********************************* Nodes ***********************************/

    private Node2D projectileSpawnPoint;
    private Node2D shotDirectionView;

    /*****************************************************************************/
    /******************************* Properties *********************************/

    [Export] private int shotCooldownTime = 500;
    private ulong cooldownUntil = 0;

    /****************************************************************************/
    /******************************* Methods ************************************/

    public override void _Ready()
    {
        base._Ready();

        // Nodes init
        projectileSpawnPoint = GetNode<Node2D>("ProjectileSpawnPoint");
        shotDirectionView = GetNode<Node2D>("ProjectileSpawnPoint/ShotDirection");
    }

    public override void _Process(double delta)
    {
        base._Process(delta);
        HandleMovement();
        HandleShooting();
        pickAnimation();

        // Rotate the shot direction to mouse position
        if (!IsGameStarted() || IsDead() || GetLevelIntroStatus() != LevelIntroStatus.LEVEL_STARTED)
            shotDirectionView.Hide();
        else
        {
            shotDirectionView.Show();

            if (IsControllerMode())
                shotDirectionView.LookAt(projectileSpawnPoint.GlobalPosition - GetControllerAimDirection());
            else
                shotDirectionView.LookAt(GetGlobalMousePosition());
        }
    }

    private Vector2 GetControllerAimDirection()
    {
        Vector2 controllerDirection = new Vector2(
            Input.GetActionStrength("controller_aim_left") - Input.GetActionStrength("controller_aim_right"), 
            Input.GetActionStrength("controller_aim_up") - Input.GetActionStrength("controller_aim_down")
        );
        
        return controllerDirection.Normalized();
    }

    private void HandleShooting()
    {
        if (Input.IsActionJustPressed("fire") && !isDead && IsGameStarted() && !IsGamePaused() &&
            GetLevelIntroStatus() == LevelIntroStatus.LEVEL_STARTED && cooldownUntil < Time.GetTicksMsec())
        {
            Projectile projectile = (Projectile)GD.Load<PackedScene>("res://GameObjects/Projectile.tscn").Instantiate();
            projectile.Shooter = this;

            if (IsControllerMode())
            {
                if (GetControllerAimDirection() != Vector2.Zero)
                    projectile.Direction = -GetControllerAimDirection();
                else
                    projectile.Direction = Vector2.Left;
            }
            else
                projectile.Direction = (GetGlobalMousePosition() - GlobalPosition).Normalized();
            
            projectile.GlobalPosition = projectileSpawnPoint.GlobalPosition;
            GetNode("/root/Main/Level/Projectiles").AddChild(projectile);
            cooldownUntil = Time.GetTicksMsec() + (ulong)shotCooldownTime;
            EmitSignal("PlayerShot");
        }
    }

    private void HandleMovement()
    {
        if (GetLevelIntroStatus() == LevelIntroStatus.LEVEL_STARTED)
        {
            if (!isDead && IsGameStarted() && !IsGamePaused())
            {
                moveDirection = new Vector2(
                    Input.GetActionStrength("move_right") - Input.GetActionStrength("move_left"),
                    Input.GetActionStrength("move_down") - Input.GetActionStrength("move_up")
                );
            }
        }

        if (!isDead)
        {
            Velocity = moveDirection.Normalized() * Speed;
            MoveAndSlide();
        }
    }

    protected override void pickAnimation()
    {
        string targetAnimation = "";

        if (IsGameStarted() && !IsGamePaused())
        {
            if (!isDead)
            {
                if (Velocity.X > 0)
                    targetAnimation = "RunRight";
                else if (Velocity.X < 0)
                    targetAnimation = "RunLeft";
                else if (Velocity.Y != 0)
                    targetAnimation = "RunVertical";

                if (Velocity == Vector2.Zero)
                    targetAnimation = "Idle";
            }
        }

        if (targetAnimation != characterSprite.Animation)
            characterSprite.Play(targetAnimation);
    }

    public void SetDirection(Vector2 direction)
    {
        moveDirection = direction;
    }

    public void Reset()
    {
        isDead = false;
        characterSprite.Show();
        deathSprite.Hide();
        hitArea.Monitoring = true;
        GetNode<CollisionShape2D>("Collision").Disabled = false;
    }
}