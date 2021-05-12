%value = 50;
%anotherValue = 25;

if (%value % 5 == 0)
{
    echo("No Remainder");
}
else if (%value == 23)
{
    echo("Remainder, Value is 23");
}
else
{
    echo("All other values not divisible by 5");

    if (%anotherValue != 25)
    {
        echo("The other value is not 25");
    }
}
