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
