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
    private AudioStreamPlayer StepsPlayer;

    /*****************************************************************************/
    /******************************* Properties *********************************/

    [Export] private int shotCooldownTime = 1000;
    private ulong cooldownUntil = 0;

    /****************************************************************************/
    /******************************* Methods ************************************/

    public override void _Ready()
    {
        base._Ready();

        // Nodes init
        projectileSpawnPoint = GetNode<Node2D>("ProjectileSpawnPoint");
        shotDirectionView = GetNode<Node2D>("ProjectileSpawnPoint/ShotDirection");
        StepsPlayer = GetNode<AudioStreamPlayer>("StepsPlayer");
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
            shotDirectionView.LookAt(GetGlobalMousePosition());
        }
        
        // Steps
        if (!IsGamePaused() && !IsDead() && velocity != Vector2.Zero)
            StepsPlayer.VolumeDb = 0;
        else
            StepsPlayer.VolumeDb = -80;
    }

    private void HandleShooting()
    {
        if (Input.IsActionJustPressed("fire") && !isDead && IsGameStarted() && !IsGamePaused() &&
            GetLevelIntroStatus() == LevelIntroStatus.LEVEL_STARTED && cooldownUntil < Time.GetTicksMsec())
        {
            Projectile projectile = (Projectile)GD.Load<PackedScene>("res://GameObjects/Projectile.tscn").Instantiate();
            projectile.Shooter = this;
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
            velocity = moveDirection.Normalized() * Speed;
            MoveAndCollide(velocity * (float)delta);
        }
    }

    protected override void pickAnimation()
    {
        string targetAnimation = "";

        if (IsGameStarted() && !IsGamePaused())
        {
            if (!isDead)
            {
                if (velocity.X > 0)
                    targetAnimation = "RunRight";
                else if (velocity.X < 0)
                    targetAnimation = "RunLeft";
                else if (velocity.Y != 0)
                    targetAnimation = "RunVertical";

                if (velocity == Vector2.Zero)
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