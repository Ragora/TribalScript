function simple()
{
    echo("Simple function");
}

function simpleParams(%one, %two)
{
    echo(%one SPC %two SPC "Parameters");
}

function simple::namespaced()
{
    echo("Simple::namespaced");
}

function simple::namespacedParams(%one, %two)
{
    echo(%one SPC %two SPC "Namespaced Parameters");
}
