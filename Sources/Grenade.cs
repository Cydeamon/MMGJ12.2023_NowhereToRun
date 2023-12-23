using Godot;
using System;
using System.Diagnostics;
using NowhereToRun.Sources.Characters;

public partial class Grenade : StaticBody2D
{
    /*****************************************************************************/
    /********************************** Nodes ************************************/

    private Sprite2D blastSprite;
    private Sprite2D grenadeSprite;
    private AudioStreamPlayer explosionPlayer;

    /*****************************************************************************/
    /******************************* Properties **********************************/

    [Export] private int minThrowPower = 10;
    [Export] private int maxThrowPower = 100;
    private Vector2 velocity = Vector2.Zero;
    
    public override void _Ready()
    {
        // Nodes init
        blastSprite = GetNode<Sprite2D>("Blast");  
        grenadeSprite = GetNode<Sprite2D>("Grenade");
        explosionPlayer = GetNode<AudioStreamPlayer>("ExplosionPlayer");
        
        grenadeSprite.Show();
    }
    
    public void Throw(Vector2 direction)
    {
        velocity = direction.Normalized() * GD.RandRange(minThrowPower, maxThrowPower);
    }

    public override void _Process(double delta)
    {
        base._Process(delta);
        explosionPlayer.VolumeDb = GlobalGameState.SoundsVolume;
        
        velocity = velocity.Lerp(Vector2.Zero, (float)(5 * delta));
        KinematicCollision2D collision = MoveAndCollide(velocity * (float)delta);

        if (collision != null)
            velocity = velocity.Bounce(collision.GetNormal());
    }

    private void OnExplosionTimerTimeout()
    {
        GetNode<AnimationPlayer>("AnimationPlayer").Play("Blast");
    }

    private void OnBlast()
    {
        // Determine who is in range and damage them
        Area2D explosionArea = GetNode<Area2D>("ExplosionArea");
        
        for (int i = 0; i < explosionArea.GetOverlappingBodies().Count; i++)
            if (explosionArea.GetOverlappingBodies()[i] is Character character)
                character.BlastHit(GlobalPosition); 
    }
}