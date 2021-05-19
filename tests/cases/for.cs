function forLoopTest(%initializer)
{
    $global = %initializer;
    for(%iteration=0; %iteration < 10; %iteration++)
    {
        echo("Current Global: " @ $global);
        $global = $global + %iteration;
    }
    echo("Result: " @ $global);
}
forLoopTest(5);
