for (%x = 0; %x < 50; %x++)
{
    echo(%x);

    for (%y = 0; %y < 100; %y++)
    {
        echo(%y);
    }
}

function simple()
{
    echo("Simple function");
}

function simpleParams(%one, %two)
{
    echo(%one SPC %two SPC "Parameters");

    for (%iteration = %one; %iteration < %two; %iteration++)
    {
        echo(%iteration);

        if (%iteration == 1)
        {
            echo("Got One");
            break;
        }
    }

    return 0;
}

function simple::namespaced()
{
    echo("Simple::namespaced");

    %value = 100;
    while (true)
    {
        switch(%value)
        {
            case 50:
                echo("Fifty");
            case 25:
                echo("Twenty-Five");
            case 1:
                break; // Leave the while loop - breaks in Torque aren't used for switch statements
            default:
                echo("None of the Above");
        }

        %value--;
    }

    %value = %value > 1 ? 100 : -100;
    return %value;
}

function simple::namespacedParams(%one, %two)
{
    echo(%one SPC %two SPC "Namespaced Parameters");

    return simpleParams(%one, %two);
}

datablock SensorData(PlayerSensor)
{
    detects = true;
    detectsUsingLOS = true;
    detectsPassiveJammed = true;
    detectRadius = 2000;
    detectionPings = false;
    detectsFOVOnly = true;
    detectFOVPercent = 1.3;
    useObjectFOV = true;
};

// Datablocks can inherit from other datablocks
datablock ParticleData(BiodermArmorJetParticle) : HumanArmorJetParticle
{
    colors[0]     = "0.50 0.48 0.36 1.0";
    colors[1]     = "0.50 0.48 0.36 0";
};


package TestPackage
{
    function My::PackagedFunction(%param)
    {
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

        return %myObject;
    }
};
activatePackage(TestPackage);
