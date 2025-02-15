#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>
#include <memory>


/*
 * Please fix all memory leaks / ownership problems using smart pointers.
 * (Verify by running the program with valgrind!)
 *
 * In the *essentials course*:
 * - Work on problem1() and problem2()
 * - You can have a look at problem3() if interested
 *
 * In the *advanced course*:
 * - Work on problem1() to problem4()
 *
 * In main() at the bottom, comment in the different parts as you progress through the
 * exercise.
 *
 * Remember that:
 * - The unique ownership of data is expressed using unique_ptr.
 * - "Observer" access without ownership is expressed using raw pointers, references or spans.
 * - Shared ownership to data is expressed using shared_ptr.
 */


/* --------------------------------------------------------------------------------------------
 * 1: Always use smart pointers instead of new.
 *
 * A frequent source of leaks is a function that terminates in an unexpected way.
 *
 * - Fix the leak using a smart pointer.
 * - The arguments of sumEntries() don't need to change, as it has only read access.
 * --------------------------------------------------------------------------------------------
 */

// Note how we are using a span pointing to "double const" to ensure that the
// data can only be read. You don't need to change anything in this function.
double sumEntries(std::span<double const> range) {
    // Simulate an error
    throw std::invalid_argument("Error when summing over data.");

    return std::reduce(range.begin(), range.end());
}

// Often, data are owned by one entity, and merely used by others. In this case, we hand the
// data to sumEntries() for reading, so the ownership stays with this function. Unfortunately,
// something goes wrong and we didn't use smart pointers. Understand and fix the memory leak.
void doStuffWithData() {
    // auto data = new std::array<double, 10000>{};
	auto data = std::make_unique<std::array<double,10000>>();

    sumEntries(*data);

    // delete data;
}


void problem1() {
   try {
       doStuffWithData();
   } catch (const std::exception& e) {
       std::cerr << "problem1() terminated with exception: \"" << e.what()
               << "\" Check for memory leaks.\n";
   }
}



/* --------------------------------------------------------------------------------------------
 * 2: Storing unique_ptr in collections.
 *
 * Often, one has to store pointers to objects in collections. Fix the memory leaks using
 * unique_ptr.
 *
 * Notes:
 * - Factory functions should return objects either directly or using smart pointers.
 *   This is good practice, because it clearly shows who owns an object. Fix the return type
 *   of the factory function.
 * - The vector should own the objects, so try to store them using smart pointers.
 * - Since the change function doesn't accept smart pointers, find a solution to pass the
 *   objects.
 *   Note that this works without shared_ptr!
 * --------------------------------------------------------------------------------------------
 */

// This is a large object. We maybe shouldn't copy it, so using a pointer is advisable to
// pass it around.
struct LargeObject {
    std::array<double, 100000> fData;
};

// A factory function to create large objects.
std::unique_ptr<LargeObject> createLargeObject() {
	// This just allocates a new large object on the heap and return the address to that
	// memory block.
    // auto object = new LargeObject();
	auto object = std::make_unique<LargeObject>();
    // Imagine there is more setup steps of "object" here
    // ...

    return object;
}

// A function to do something with the objects.
// Note that since we don't own the object, we don't need a smart pointer as argument.
void changeLargeObject(LargeObject& object) {
    object.fData[0] = 1.;
}

void problem2() {
	// Pointers to objects in a collection (in this case std::vector)
    std::vector<std::unique_ptr<LargeObject>> largeObjects;

    for (unsigned int i=0; i < 10; ++i) {
		// Allocates new large objects and push the addresses into the vector
        auto newObj = createLargeObject();
		// Now newObj owns the resource allocated with createLargeObject() which is a unique
		// pointer (only one owner allowed) so we cannot just push_back and copy newObj into
		// the vector but must transfer the ownership to the other unique_ptr inside the
		// vector.
        largeObjects.push_back(std::move(newObj));
    }

    for (const auto& obj : largeObjects) {
        changeLargeObject(*obj);
    }

	// Memory leak because we didn't delete each large object that we allocated.
	// How should a smart pointer solve this issue?
}



/* --------------------------------------------------------------------------------------------
 * 3: Shared ownership.
 *
 * Most of the time, ownership can be solved by having one owner (with unique_ptr) and one or
 * more observers (raw pointers or references). Sometimes, we need to truly share data, though.
 *
 * Here is an example of a completely messed up ownership model. It leaks about 1/10 of the
 * times it is invoked.
 * - Verify this by running it in a loop using a command like:
 * while true; do valgrind --leak-check=full --track-origins=yes ./smartPointers 2>&1 |
 * grep -B 5 -A 5 problem3 && exit 1; done
 * - Fix the ownership model using shared_ptr!
 *   - Convert the vectors to holding shared_ptr.
 *   - Fix the arguments of the functions.
 *   - Speed optimisation:
 *     Make sure that you don't create & destroy a shared_ptr in the for loop in problem3()
 *     and when calling processElement().
 * --------------------------------------------------------------------------------------------
 */

// This removes the element in the middle of the vector.
void removeMiddle(std::vector<LargeObject*>& collection) {
    auto middlePosition = collection.begin() + collection.size()/2;

    // Must not delete element when erasing from collection, because it's also in the copy ...
    collection.erase(middlePosition);
}

// This removes a random element.
// Note that this leaks if the element happens to be the same
// that's removed above ...
void removeRandom(std::vector<LargeObject*>& collection) {
    auto pos = collection.begin() + time(nullptr) % collection.size();

    collection.erase(pos);
}

// Do something with an element.
// Just a dummy function, for you to figure out how to pass an object
// managed by a shared_ptr to a function.
void processElement(const LargeObject* /*element*/) { }


// We have pointers to objects in two different collections. We work a bit with
// the collections, and then we try to terminate leak free. Without a shared ownership
// model, this becomes a mess.
void problem3() {
    // Let's generate a vector with 10 pointers to LargeObject
    std::vector<LargeObject*> objVector(10);
    for (auto& ptr : objVector) {
        ptr = new LargeObject();
    }

    // Let's copy it
    std::vector<LargeObject*> objVectorCopy(objVector);


    // Now we work with the objects:
    removeMiddle(objVector);
    removeRandom(objVectorCopy);
    // ...
    // ...
    for (auto elm : objVector) {
        processElement(elm);
    }


    // Now try to figure out what has to be deleted. It's a mess ...
    // Fix using shared_ptr, so the following code becomes unnecessary:
    for (auto objPtr : objVector) {
        delete objPtr;
    }

    for (auto objPtr : objVectorCopy) {
        // If the element is in the original collection, it was already deleted.
        if (std::find(objVector.begin(), objVector.end(), objPtr) == objVector.end()) {
            delete objPtr;
        }
    }
}



/* --------------------------------------------------------------------------------------------
 * 4: Smart pointers as class members.
 *
 * Class members that are pointers can quickly become a problem.
 * Firstly, if only raw pointers are used, the intended ownership is unclear.
 * Secondly, it's easy to overlook that a member has to be deleted.
 * Thirdly, pointer members usually require you to implement copy or move constructors and
 * assignment operators (--> rule of 3, rule of 5).
 * Since C++-11, one can solve a few of those problems using smart pointers.
 *
 * 4.1:
 * The class "Owner" owns some data, but it is broken. If you copy it like in
 * problem4_1(), you have two pointers pointing to the same data, but both instances think
 * that they own the data.
 *
 * Tasks:
 * - Comment in problem4_1() in main().
 * - It likely crashes. Verify this. You can also try running valgrind ./smartPointers, it
 *   should give you some hints as to what's happening.
 * - Fix the Owner class by using a shared_ptr for its _largeObj, which we can copy as much
 *   as we want.
 * - Run the fixed program.
 * - Note: Once shared_ptr is in use, you can also use the default destructor.
 *
 * 4.2: **BONUS**
 * Let's use a weak_ptr now to observe a shared_ptr.
 * These are used to observe a shared_ptr, but unlike the shared_ptr, they don't prevent the
 * deletion of the underlying object if all shared_ptr go out of scope.
 * To *use* the observed data, one has to create a shared_ptr from the weak_ptr, so that it
 * is guaranteed that the underlying object is alive.
 *
 * In our case, the observer class wants to observe the data of the owner, but it doesn't
 * need to own it.
 * To do this, we use a weak pointer.
 *
 * Tasks:
 * - Comment in problem4_2() in main().
 * - Investigate the crash. Use a debugger, run in valgrind, compile with -fsanitize=address
 * - Rewrite the interface of Owner::getData() such that the observer can see the shared_ptr
 *   to the large object.
 * - Set up the Observer such that it stores a weak pointer that observes the large object.
 * - In Observer::processData(), access the weak pointer, and use the data *only* if the
 *   memory is still alive.
 *   Note: What you need is weak_ptr::lock(). Check out the documentation and the example at
 *   the bottom: https://en.cppreference.com/w/cpp/memory/weak_ptr/lock
 * --------------------------------------------------------------------------------------------
 */

class Owner {
public:
    Owner() :
        _largeObj(new LargeObject()) { }

    ~Owner() {
        std::cout << "problem4(): Owner " << this << " is deallocating " << _largeObj << ".\n";
        delete _largeObj;
    }

    const LargeObject* getData() const {
        return _largeObj;
    }

private:
    LargeObject* _largeObj;
};


void problem4_1() {
    std::vector<Owner> owners;

    for (int i=0; i < 5; ++i) {
        Owner owner;
        owners.push_back(owner);
    }

    /* Now we have a problem:
     * We created Owner instances on the stack, and copied them into the vector.
     * When the instances on the stack are destroyed, the memory is deallocated.
     * All copies in the vector now point to the deallocated memory!
     * We could fix this using copy constructors (but we don't want to copy the data),
     * using move semantics or using shared_ptr.
     * Here, we want to go for shared_ptr.
     */
}


class Observer {
public:
    Observer(const Owner& owner) :
        _largeObj(owner.getData()) { }

    double getValue() const {
        if (_largeObj) {
            return _largeObj->fData[0];
        }

        return -1.;
    }

private:
    const LargeObject* _largeObj; // We don't own this.
};



void problem4_2() {
    // We directly construct 5 owners inside the vector to get around problem4_1:
    std::vector<Owner> owners(5);

    // Let's now fill the other vector with observers:
    std::vector<Observer> observers;
    observers.reserve(owners.size());
    for (const auto& owner : owners) {
        observers.emplace_back(owner);
    }

    // Now let's destroy a few of the data owners:
    owners.resize(3);

    std::cout << "Values of the observers:\n\t";
    for (const auto& observer : observers) {
        // Problem: We don't know if the data is alive ...
        // TODO: Fix Observer!
        // std::cout << observer.getValue() << " ";
    }
    std::cout << "\n";
}


int main() {
    // problem1();
    problem2();
    // problem3();
    // problem4_1();
    // problem4_2();
}
