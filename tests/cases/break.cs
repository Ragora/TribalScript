$result::break = 0;
for (%i=0;%i<10;%i++)
{
    $result::break += %i;

    if (%i == 5)
    {
        break;
    }
}
