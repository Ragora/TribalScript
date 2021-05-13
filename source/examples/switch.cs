%value = 5;
$Some::Global = 2;

switch(%value)
{
    case 1:
        echo("one");
    case $Some::Global:
        echo("two");
    case 5:
        echo("five");
    default:
        echo("Default case");
}

// In Torque, switch$ specifically is used for switching on string values
%string = "My String";
$Some::Global::String = "Another String";

switch$(%string)
{
    case $Some::Global::String:
        echo("Some Global String");
    case "two":
        echo("Two");
    default:
        echo("Default");
}

// In Torque, the case values are entire expressions
switch(%value)
{
    case (5+5)*2:
        echo("Case 1");
    case (2/2)*3 or (1+1)/2:
        echo("Case 2");
    default:
        echo("Default Case");
}
