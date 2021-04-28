// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.

using UnrealBuildTool;
using System.Collections.Generic;

public class VehicleSeriesTarget : TargetRules
{
	public VehicleSeriesTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "VehicleSeries" } );
	}
}
