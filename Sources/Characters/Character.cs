using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public abstract partial class Character : StaticBody2D
{
    /****************************************************************************/
    /********************************* Signals **********************************/
    
    [Signal] public delegate void KilledEventHandler();
    
    /***************************************************************************/
    /********************************* Nodes ***********************************/

    protected AnimatedSprite2D characterSprite;
    protected Area2D hitArea;
    protected Area2D bloodDropArea;
    protected CollisionShape2D bloodDropAreaBigCollision;
    protected CollisionShape2D bloodDropAreaSmallCollision;
    protected Timer bleedingTimer;
    protected Sprite2D bloodDrawCanvas;
    protected Sprite2D deathSprite;


    /***************************************************************************/
    /******************************* Properties ********************************/

    [Export] public int Speed = 50;
    protected double delta;
    protected bool isDead = false;
    protected Vector2 velocity = Vector2.Zero;
    protected Vector2 moveDirection = Vector2.Zero;
    protected float bloodDropRate = 0.02f;
    protected float lastBloodDropTime = 0f;
    protected bool isBleeding = false;
    protected readonly Random random = new Random();
    protected Color bloodColor = new Color("c4181f");


    /***************************************************************************/
    /******************************* Methods **********************************/


    protected abstract void pickAnimation();

    public override void _Ready()
    {
        // Init nodes
        characterSprite = GetNode<AnimatedSprite2D>("Sprite");
        hitArea = GetNode<Area2D>("HitArea");
        bloodDropArea = GetNode<Area2D>("BloodDropArea");
        bleedingTimer = GetNode<Timer>("BleedingTimer");
        bloodDropAreaBigCollision = GetNode<CollisionShape2D>("BloodDropArea/BigShape");
        bloodDropAreaSmallCollision = GetNode<CollisionShape2D>("BloodDropArea/SmallShape");
        bloodDrawCanvas = GetNode<Sprite2D>("/root/Main/Level/BloodDrawSprite");
        deathSprite = GetNode<Sprite2D>("DeathSprite");
        
        // Random init values
        CircleShape2D bloodPoolShape = new CircleShape2D();
        bloodPoolShape.Radius = (random.NextSingle() * 4f) + 2f;
        bloodDropAreaBigCollision.Shape = bloodPoolShape;

        bleedingTimer.WaitTime = (random.NextSingle() * 4) + 1;

    }

    public override void _Process(double delta)
    {
        this.delta = delta;

        if (isBleeding)
        {
            if (Time.GetTicksMsec() - lastBloodDropTime > bloodDropRate * 1000)
            {
                DropBlood();
                lastBloodDropTime = Time.GetTicksMsec();
            }
        }

        if (isDead)
        {
            if (!deathSprite.Visible)
                showDeathSprite();

            if (velocity != Vector2.Zero)
            {
                velocity = velocity.Lerp(Vector2.Zero, (float)(5 * delta));
                KinematicCollision2D collision = MoveAndCollide(velocity * (float)delta);

                if (collision != null)
                    velocity = velocity.Bounce(collision.GetNormal());
                
                if (Math.Abs(velocity.X) < 0.5)
                    velocity.X = 0;

                if (Math.Abs(velocity.Y) < 0.5)
                    velocity.Y = 0;
                
                if (velocity == Vector2.Zero)
                    GetNode<CollisionShape2D>("Collision").Disabled = true;
            }
        }
    }

    private void showDeathSprite()
    {
        characterSprite.Hide();
        deathSprite.Show();

        int framesNum = deathSprite.Texture.GetWidth() / 8;
        int randomFrame = random.Next(framesNum);

        deathSprite.RegionRect = new Rect2(
            new Vector2(randomFrame * 8, 0),
            new Vector2(8, 8)
        );
    }

    protected void startBleeding()
    {
        isBleeding = true;
        bleedingTimer.Start();
    }

    protected void stopBleeding()
    {
        isBleeding = false;
    }

    public void DropBlood()
    {
        if (IsGameStarted() && !IsGamePaused() && isBleeding)
        {
            bool drawPositionWithinRadius = false;
            Vector2 drawPosition = Vector2.Zero;

            while (!drawPositionWithinRadius)
            {
                CircleShape2D bigShape = bloodDropAreaBigCollision.Shape as CircleShape2D;
                CircleShape2D smallShape = bloodDropAreaSmallCollision.Shape as CircleShape2D;

                float radius = bigShape.Radius;

                if (velocity != Vector2.Zero)
                    radius = smallShape.Radius;

                drawPosition = new Vector2(
                    random.Next((int)(radius * 2)) - radius,
                    random.Next((int)(radius * 2)) - radius
                );

                drawPositionWithinRadius = drawPosition.Length() < radius;
                drawPosition += bloodDropAreaBigCollision.GlobalPosition;
            }

            // Draw blood drop
            Image img = bloodDrawCanvas.Texture.GetImage();

            if (drawPosition.X >= 0 && drawPosition.X <= img.GetWidth() &&
                drawPosition.Y >= 0 && drawPosition.Y <= img.GetHeight())
            {
                img.SetPixelv((Vector2I)drawPosition, bloodColor);
                bloodDrawCanvas.Texture = ImageTexture.CreateFromImage(img);
            }
        }
    }

    public void BulletHit(Vector2 direction)
    {
        if (!isDead)
        {
            die();
            velocity = direction * (random.Next(275) + 25);
            EmitSignal("Killed");
        }
    }
    
    public void BlastHit(Vector2 explosionPosition)
    {
        if (!isDead)
        {
            die();
            velocity = (GlobalPosition - explosionPosition).Normalized() * (random.Next(275) + 25);
            EmitSignal("Killed");
        }
    }

    protected void die()
    {
        isDead = true;
        startBleeding();
        hitArea.Monitoring = false;
    }

    /***************************************************************************/
    /*************************** Getters & Setters *****************************/

    public bool IsDead() => isDead;
}