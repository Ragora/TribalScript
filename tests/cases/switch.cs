function switchCaseTest(%index)
{
    $some::Global = 500;
    switch (%index)
    {
        case 1 or 2 or 3:
            return 5;
        case $some::global:
            return 10;
    }

    return -10;
}

$global::one = switchCaseTest(1);
$global::two = switchCaseTest(2);
$global::three = switchCaseTest(500);
$global::four = switchCaseTest(600);
