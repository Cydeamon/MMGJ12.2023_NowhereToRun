using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public partial class Player : Character
{

	/****************************************************************************/
	/********************************* Nodes ***********************************/

	private Node2D projectileSpawnPoint;
	
	/*****************************************************************************/
	/******************************* Properties *********************************/
	
	
	
	/****************************************************************************/
	/******************************* Methods ************************************/

	public override void _Ready()
	{
		base._Ready();
		
		// Nodes init
		projectileSpawnPoint = GetNode<Node2D>("ProjectileSpawnPoint");
	}
	
	public override void _Process(double delta)
	{
		base._Process(delta);
		HandleMovement();
		HandleShooting();
		pickAnimation();
	}

	private void HandleShooting()
	{
		if (Input.IsActionJustPressed("fire") && !isDead && IsGameStarted() && !IsGamePaused())
		{
			Projectile projectile = (Projectile) GD.Load<PackedScene>("res://GameObjects/Projectile.tscn").Instantiate();
			projectile.HurtPlayer = false;
			projectile.Direction = (GetGlobalMousePosition() - GlobalPosition).Normalized();
			projectile.GlobalPosition = projectileSpawnPoint.GlobalPosition;
			GetNode("/root/Main/Level/Projectiles").AddChild(projectile);
		}
	}

	private void HandleMovement()
	{
		if (!isDead && IsGameStarted() && !IsGamePaused())
		{
			moveDirection = new Vector2(
				Input.GetActionStrength("move_right") - Input.GetActionStrength("move_left"),
				Input.GetActionStrength("move_down") - Input.GetActionStrength("move_up")
			);
			
			velocity = moveDirection * Speed;
			MoveAndCollide(velocity * (float) delta);
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
}