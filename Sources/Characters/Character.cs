using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public partial class Character : StaticBody2D
{
    /***************************************************************************/
    /********************************* Nodes ***********************************/

    protected AnimatedSprite2D characterSprite;
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


    public override void _Ready()
    {
        // Init nodes
        characterSprite = GetNode<AnimatedSprite2D>("Sprite");
        bloodDropArea = GetNode<Area2D>("BloodDropArea");
        bleedingTimer = GetNode<Timer>("BleedingTimer");
        bloodDropAreaBigCollision = GetNode<CollisionShape2D>("BloodDropArea/BigShape");
        bloodDropAreaSmallCollision = GetNode<CollisionShape2D>("BloodDropArea/SmallShape");
        bloodDrawCanvas = GetNode<Sprite2D>("/root/Main/Level/BloodDrawSprite");
        deathSprite = GetNode<Sprite2D>("DeathSprite");
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
            }

            Image img = bloodDrawCanvas.Texture.GetImage();
            img.SetPixelv((Vector2I)(bloodDropAreaBigCollision.GlobalPosition + drawPosition), bloodColor);
            bloodDrawCanvas.Texture = ImageTexture.CreateFromImage(img);
        }
    }
}