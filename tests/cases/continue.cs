$result::continue = 0;
for (%i=0;%i<10;%i++)
{
    $result::continue += %i;

    if (%i >= 5)
    {
        continue;
    }
}
