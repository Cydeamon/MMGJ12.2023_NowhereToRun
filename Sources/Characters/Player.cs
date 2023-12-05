using System;
using Godot;
using static NowhereToRun.Sources.Main;

namespace NowhereToRun.Sources.Characters;

public partial class Player : Character
{
	/*****************************************************************************/
	/******************************* Properties *********************************/
	
	
	
	/*****************************************************************************/
	/******************************* Methods ***********************************/

	public override void _Process(double delta)
	{
		base._Process(delta);
		HandleMovement();
		PickAnimation();
	}

	public override void _Ready()
	{
		base._Ready();
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

	private void PickAnimation()
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