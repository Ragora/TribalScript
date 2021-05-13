$Simple = 50;
$Name::Spaced = 3.14;
$Indexed[$Simple + 123] = "String";

// $Name::Spaced[50] returns "" in Torque because the index does not exist - in fact translates to a key like name_spaced_50
$Name::Spaced::Indexed[$Name::Spaced[50]] = "New String";
