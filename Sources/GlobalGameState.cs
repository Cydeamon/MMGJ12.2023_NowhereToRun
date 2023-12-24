using Godot;
using System;

[GlobalClass]
public partial class GlobalGameState : Node
{
	private static float MusicVolume;
	private static float SoundsVolume;
	private static bool Fullscreen;
	
	/*******************************************/
	/***************** Getters *****************/

	public static bool GetFullscreen() 
	{
		return Fullscreen;
	}
	
	public static float GetMusicVolume()
	{
		if (MusicVolume == -50.0f)
			return -80;

		return MusicVolume;
	}
	
	public static float GetSoundsVolume()
	{
		if (SoundsVolume == -50.0f)
			return -80;

		return SoundsVolume;
	}

	/*******************************************/
	/***************** Setters *****************/
	
	public static void SetFullscreen(bool value)
	{
		Fullscreen = value;
	}
	
	public static void SetMusicVolume(float value)
	{
		if (value < -50.0f)
			value = -50.0f;
		else if (value > 0.0f)
			value = 0.0f;
		
		MusicVolume = value;
	}
	
	public static void SetSoundsVolume(float value)
	{
		if (value < -50.0f)
			value = -50.0f;
		else if (value > 0.0f)
			value = 0.0f;
		
		SoundsVolume = value;
	}
}
