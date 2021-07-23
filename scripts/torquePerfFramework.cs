function localVariableBenchMark(%samples)
{
	echo("localVariableBenchMark: Begin");

	%result = "";
	for (%sample = 0; %sample < %samples; %sample++)
	{
		%start = getRealTime();
		for (%iteration = 0; %iteration < 2000000; %iteration++)
		{
		}
		%delta = getRealTime() - %start;

		if (%result $= "")
		{
			%result = %delta;
		}
		else
		{
			%result = %result @ "," @ %delta;
		}
	}

	echo("localVariableBenchMark: End");
	return %result;
}

function benchFunction()
{
}

function functionCallOverhead(%samples)
{
	echo("functionCallOverhead: Begin");

	%result = "";
	for (%sample = 0; %sample < %samples; %sample++)
	{
		%start = getRealTime();
		for (%iteration = 0; %iteration < 2000000; %iteration++)
		{
			benchFunction();
		}
		%delta = getRealTime() - %start;

		if (%result $= "")
		{
			%result = %delta;
		}
		else
		{
			%result = %result @ "," @ %delta;
		}
	}

	echo("functionCallOverhead: End");
	return %result;
}

function benchFunctionThreeParameter(%a, %b, %c)
{
}

function functionCallThreeParameterOverhead(%samples)
{
	echo("functionCallThreeParameterOverhead: Begin");

	%result = "";
	for (%sample = 0; %sample < %samples; %sample++)
	{
		%start = getRealTime();
		for (%iteration = 0; %iteration < 2000000; %iteration++)
		{
			benchFunctionThreeParameter(1, 2, 3);
		}
		%delta = getRealTime() - %start;

		if (%result $= "")
		{
			%result = %delta;
		}
		else
		{
			%result = %result @ "," @ %delta;
		}
	}

	echo("functionCallThreeParameterOverhead: End");
	return %result;
}

function objectOverhead(%samples)
{
	echo("objectOverhead: Begin");

	%result = "";
	for (%sample = 0; %sample < %samples; %sample++)
	{
		%start = getRealTime();
		for (%iteration = 0; %iteration < 2000000; %iteration++)
		{
			%object = new FileObject();
			%object.delete();
		}
		%delta = getRealTime() - %start;

		if (%result $= "")
		{
			%result = %delta;
		}
		else
		{
			%result = %result @ "," @ %delta;
		}
	}

	echo("objectOverhead: End");
	return %result;
}

function arrayOverhead(%samples)
{
	echo("arrayOverhead: Begin");

	%result = "";
	for (%sample = 0; %sample < %samples; %sample++)
	{
		%start = getRealTime();

		for (%x = 0; %x < 1000; %x++)
		{
            for (%y = 0; %y < 1000; %y++)
            {
                %array[%x, %y] = 1;
            }
		}
		%delta = getRealTime() - %start;

		if (%result $= "")
		{
			%result = %delta;
		}
		else
		{
			%result = %result @ "," @ %delta;
		}
	}

	echo("arrayOverhead: End");
	return %result;
}

// Run all tests
%sampleCount = 10;
echo("Running benchmarks ...");
%localVariableResult = localVariableBenchMark(%sampleCount);
%functionCallOverheadResult = functionCallOverhead(%sampleCount);
%functionCallThreeParameterOverheadResult = functionCallThreeParameterOverhead(%sampleCount);
%objectOverheadResult = objectOverhead(%sampleCount);
%arrayOverheadResult = arrayOverhead(%sampleCount);

// Output to disk
%handle = new FileObject();
%handle.openForWrite("result.csv");
%handle.write("" NL "");
%handle.write("" NL "");

// Write results
%handle.write("Local Variable," @ %localVariableResult NL "");
%handle.write("Function Call Overhead," @ %functionCallOverheadResult NL "");
%handle.write("Function Call Three Parameter Overhead," @ %functionCallThreeParameterOverheadResult NL "");
%handle.write("Object Overhead," @ %objectOverheadResult NL "");
%handle.write("Array Overhead," @ %arrayOverheadResult NL "");

// Finish write
%handle.close();
%handle.delete();

echo("Benchmark results Written to: result.csv");
