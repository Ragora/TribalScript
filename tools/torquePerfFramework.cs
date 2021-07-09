function torquePerfRawOps(%iterations)
{
    %result = 0;
    for (%i=0;%i<%iterations;%i++)
    {
        %result = %result + 1;
    }
    return %result;
}

function torquePerfFunctionCalls(%iterations)
{
    %result = 0;
    for (%i=0;%i<%iterations;%i++)
    {
        %result = %result + getRandom();
    }
    return %result;
}

function performAverages(%measuredFunction, %callCount, %param1, %param2, %param3)
{
    echo("Measuring " @ %measuredFunction @ " ... ");

    %result = 0;

    for (%i=0;%i<%callCount;%i++)
    {
        %now = getRealTime();
        call(%measuredFunction, %param1, %param2, %param3);
        %delta = getRealTime() - %now;

        %result = %result + %delta;
    }

    return %result / %callCount;
}

function torquePerfFramework()
{
    echo("Torque Performance Framework v0.1 ...");

    %rawOps = performAverages("torquePerfRawOps", 100, 1000000);
    echo("Raw Ops: " @ %rawOps @ "ms");

    %functionCalls = performAverages("torquePerfFunctionCalls", 100, 1000000);
    echo("Function Calls: " @ %functionCalls @ "ms");
}


torquePerfFramework();
