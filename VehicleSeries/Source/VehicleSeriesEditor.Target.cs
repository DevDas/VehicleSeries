// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.

using UnrealBuildTool;
using System.Collections.Generic;

public class VehicleSeriesEditorTarget : TargetRules
{
	public VehicleSeriesEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "VehicleSeries" } );
	}
}
