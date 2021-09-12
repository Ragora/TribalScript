new SimSet(Root)
{
    field = new SimGroup(field);

    new SimSet(ChildRoot)
    {
        childField = new SimGroup(childField);

        new SimSet(Child)
        {

        };
    };
};

// Set result values
$root::field = Root.field.getName();
$root::ChildRoot = Root.getObject(0).getName();
$root::childField = Root.getObject(0).childField.getName();
$root::child = Root.getObject(0).getObject(0).getName();
