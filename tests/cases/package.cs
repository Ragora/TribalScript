function getNumber()
{
    return 1;
}

package a
{
    function getNumber()
    {
        return parent::getNumber() + 1;
    }
};

package b
{
    function getNumber()
    {
        return parent::getNumber() + 1;
    }
};

$before = getNumber();
activatePackage(a);
$afterA = getNumber();
activatePackage(b);
$afterB = getNumber();
