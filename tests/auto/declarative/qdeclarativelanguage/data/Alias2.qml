import Test 1.0
import Qt 4.6

QtObject {
    property variant other
    other: MyTypeObject { id: obj }
    property alias enumAlias: obj.enumProperty;
}
