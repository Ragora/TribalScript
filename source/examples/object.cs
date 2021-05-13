// This notation is often used for mission files
new SimGroup(MissionGroup) {
		CnH_timeLimit = "25";
		cdTrack = "5";
		musicTrack = "ice";
		powerCount = "0";

    // Sub-object of MissionGroup
	new MissionArea(MissionArea) {
		area = "-600 -808 992 1632";
		flightCeiling = "2000";
		flightCeilingRange = "50";
			locked = "true";
	};
};

// However, it may be assigned to variables as well
%myObject = new SimGroup(MissionGroup) {
		CnH_timeLimit = "25";
		cdTrack = "5";
		musicTrack = "ice";
		powerCount = "0";

    // Sub-object of MissionGroup
	new MissionArea(MissionArea) {
		area = "-600 -808 992 1632";
		flightCeiling = "2000";
		flightCeilingRange = "50";
			locked = "true";
	};
};

// Though you usually get inline assignments like
%myGroup = new SimGroup(MyGroup);

// Sometimes you see this notation as well
%piece = new (StaticShape)(OptionalName) { position = "0 0 0"; };
