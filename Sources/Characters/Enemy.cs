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
    [Export] public int ShotMinTime = 1000;
    [Export] public int ShotMaxTime = 2000;
    [Export] public int GrenadeThrowMinTime = 10000;
    [Export] public int GrenadeThrowMaxTime = 20000;
    private int grenades = 2;
    private ulong nextShotTime;
    private Node2D projectileSpawnPoint;

    /****************************************************************************/
    /******************************* Level nodes ********************************/

    private Area2D EnemiesSpawnArea;
    private CollisionShape2D EnemiesSpawnAreaCollision;
    private RectangleShape2D EnemiesSpawnAreaCollisionShape;
    private Player Player;
    private Area2D LevelStartEnemiesPoint;
    private CollisionShape2D LevelStartEnemiesPointCollision;

    /****************************************************************************/
    /******************************** Methods ***********************************/


    public override void _Ready()
    {
        base._Ready();
        Speed -= 30;

        if (Type == 0)
            isEmpty = true;

        nextShotTime = Time.GetTicksMsec();

        // Nodes init
        EnemiesSpawnArea = GetNode<Area2D>("/root/Main/Level/EnemiesSpawnArea");
        EnemiesSpawnAreaCollision = EnemiesSpawnArea.GetChild<CollisionShape2D>(0);
        EnemiesSpawnAreaCollisionShape = EnemiesSpawnArea.GetChild<CollisionShape2D>(0).Shape as RectangleShape2D;
        Player = GetNode<Player>("/root/Main/Level/Player");
        LevelStartEnemiesPoint = GetNode<Area2D>("/root/Main/Level/LevelStartEnemiesPoint");
        LevelStartEnemiesPointCollision = LevelStartEnemiesPoint.GetChild<CollisionShape2D>(0);
        projectileSpawnPoint = GetNode<Node2D>("ProjectileSpawnPoint");
    }

    private void determineNextShotTime()
    {
        int shotTimeMin;
        int shotTimeMax;

        if (Type == EnemyType.PISTOL)
        {
            shotTimeMin = ShotMinTime;
            shotTimeMax = ShotMaxTime;
        }
        else
        {
            shotTimeMin = GrenadeThrowMinTime;
            shotTimeMax = GrenadeThrowMaxTime;
        }

        nextShotTime = (ulong)GD.RandRange(shotTimeMin, shotTimeMax) + Time.GetTicksMsec();
    }


    public override void _Process(double delta)
    {
        base._Process(delta);
        handleAI();
        handleMovement();
        pickAnimation();
    }

    private void handleMovement()
    {
        if (!isDead && !IsGamePaused())
        {
            velocity = moveDirection * Speed;
            KinematicCollision2D collision = MoveAndCollide(velocity * (float)delta);

            if (collision != null)
                velocity = Vector2.Zero;
        }
    }

    private void handleAI()
    {
        if (!isDead && !IsGamePaused())
        {
            /*
             * Empty enemies run towards the spawn point, when they reach it - they despawn
             * Pistol enemies run towards the player and shoot every 1-2 seconds
             * Grenade enemies run towards the player and throw a grenade at the 2-5 meters distance every 10-20 seconds
             */

            moveDirection = Vector2.Zero;

            if (GetLevelIntroStatus() != LevelIntroStatus.LEVEL_STARTED)
            {
                RectangleShape2D LevelStartEnemiesPointCollisionShape =
                    LevelStartEnemiesPointCollision.Shape as RectangleShape2D;

                float xLeft = LevelStartEnemiesPointCollision.GlobalPosition.X -
                              LevelStartEnemiesPointCollisionShape.Size.X / 2;
                float xRight = LevelStartEnemiesPointCollision.GlobalPosition.X +
                               LevelStartEnemiesPointCollisionShape.Size.X / 2;

                Vector2 targetPoint;

                if (GlobalPosition.X < xLeft)
                    targetPoint = new Vector2(xLeft, LevelStartEnemiesPointCollision.GlobalPosition.Y);
                else if (GlobalPosition.X > xRight)
                    targetPoint =
                        new Vector2(
                            xRight,
                            LevelStartEnemiesPointCollision.GlobalPosition.Y +
                            LevelStartEnemiesPointCollisionShape.Size.Y);
                else
                    targetPoint = new Vector2(GlobalPosition.X,
                                              LevelStartEnemiesPointCollision.GlobalPosition.Y +
                                              LevelStartEnemiesPointCollisionShape.Size.Y / 2);

                moveDirection = (targetPoint - GlobalPosition).Normalized();
                determineNextShotTime();
            }
            else
            {
                if (isEmpty)
                {
                    float xLeft = EnemiesSpawnAreaCollision.GlobalPosition.X -
                                  EnemiesSpawnAreaCollisionShape.Size.X / 2;
                    float xRight = EnemiesSpawnAreaCollision.GlobalPosition.X +
                                   EnemiesSpawnAreaCollisionShape.Size.X / 2;

                    Vector2 targetPoint;

                    if (GlobalPosition.X < xLeft)
                        targetPoint = new Vector2(xLeft, EnemiesSpawnAreaCollision.GlobalPosition.Y);
                    else if (GlobalPosition.X > xRight)
                        targetPoint =
                            new Vector2(
                                xRight,
                                EnemiesSpawnAreaCollision.GlobalPosition.Y + EnemiesSpawnAreaCollisionShape.Size.Y);
                    else
                        targetPoint = new Vector2(GlobalPosition.X,
                                                  EnemiesSpawnAreaCollision.GlobalPosition.Y +
                                                  EnemiesSpawnAreaCollisionShape.Size.Y / 2);

                    if (targetPoint == GlobalPosition)
                    {
                        QueueFree();
                        return;
                    }

                    moveDirection = (targetPoint - GlobalPosition).Normalized();
                }
                else
                {
                    if (!Player.IsDead())
                    {
                        if (Type == EnemyType.PISTOL)
                        {
                            if (Math.Abs(GlobalPosition.DistanceTo(Player.GlobalPosition)) > 20)
                                moveDirection = Player.GlobalPosition - GlobalPosition;

                            if (Time.GetTicksMsec() > nextShotTime &&
                                GetLevelIntroStatus() == LevelIntroStatus.LEVEL_STARTED)
                            {
                                determineNextShotTime();
                                Projectile projectile =
                                    (Projectile)GD.Load<PackedScene>("res://GameObjects/Projectile.tscn").Instantiate();
                                projectile.Shooter = this;
                                projectile.Direction = (Player.GlobalPosition - GlobalPosition).Normalized();
                                projectile.GlobalPosition = projectileSpawnPoint.GlobalPosition;
                                GetNode("/root/Main/Level/Projectiles").AddChild(projectile);
                            }
                        }

                        if (Type == EnemyType.GRENADE)
                        {
                            if (Math.Abs(GlobalPosition.DistanceTo(Player.GlobalPosition)) > 60)
                                moveDirection = Player.GlobalPosition - GlobalPosition;
                            else
                            {
                                if (Time.GetTicksMsec() > nextShotTime && !Player.IsDead() &&
                                    GetLevelIntroStatus() == LevelIntroStatus.LEVEL_STARTED)
                                {
                                    determineNextShotTime();
                                    grenades--;

                                    if (grenades <= 0)
                                        isEmpty = true;

                                    Grenade grenade = (Grenade)GD.Load<PackedScene>("res://GameObjects/Grenade.tscn")
                                                                 .Instantiate();
                                    grenade.Throw((Player.GlobalPosition - GlobalPosition).Normalized());
                                    grenade.GlobalPosition = projectileSpawnPoint.GlobalPosition;
                                    GetNode("/root/Main/Level/Projectiles").AddChild(grenade);
                                }
                            }
                        }
                    }
                }
            }

            moveDirection = moveDirection.Normalized();
        }
    }

    protected override void pickAnimation()
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

        characterSprite.Play(targetAnimation);
    }

    public bool IsEmpty()
    {
        return isEmpty;
    }
}