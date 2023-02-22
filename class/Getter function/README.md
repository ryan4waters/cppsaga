# The importance of `Getter` function of private member variables: a friend member function example

## Overview

This article explains the importance of using `Getter` functions for `private` members through a simple example of misuse of `friend` functions.

# Example: `friend` member function poses a security risk

Let's look at an example of a `friend` member function:

```cpp
class Employee {
public:
	Employee() = delete;
	explicit Employee(int id) : id(id) {}
	~Employee() = default;
	
	bool Active();
	
	friend bool UpdateToNewId();
	
private:
	int id = 0;
	int groupId = 0;
};

bool Employee::Active()
{
	// check whether or not an employee is active. Ignore source code.
}

bool UpdatToNewId()
{
	while(const Employee *employee = GetEmployee()) {
		if (!Valid(employee->id)) {
			return false;
		}
		Update(employee);
	}
	return true;
}
```

Initially, the employee class `Employee` has the employee's `id` and the group's `groupId`. The member function `Employee::Active` is responsible for checking whether the employee is still active.

One day, as the company's business grows, the `id` of the old employee is no longer applicable and needs to be upgraded. The developer adds the `friend` member function `UpdatToNewId`, which is responsible for updating the `id` of all employees (the `Update` function), and checking the validity of the old `id` before updating (the `Valid` function).

There is no problem in realizing the function of the above code. However, there are security risks. The problem is in `UpdatToNewId` function.

The `UpdatToNewId` function is a member function of the class `Employee`, so it can access all protected and private member variables ( `Employee::id` and `Employee::groupId`) in the class. However, `UpdatToNewId` actually only needs access to the `Employee::id` (when calling the `Valid` function). In this way, other private member variables of the class are exposed, which does not meet the principle of minimization in security.

## Correct way: `private` variables are accessed through `Getter` functions

The root of the above design is that the `Employee` class did not take into account the protection of `private` members at the beginning of its design. In this way, when future developers need to call the `private` member variable of the class, it is easy to do so by adding a `friend` member function.

```cpp
class Employee {
public:
	Employee() = delete;
	explicit Employee(int id) : id(id) {}
	~Employee() = delete;
	
	bool Active();
	
	int GetId() {return id;}
	int groupId {return groupId;}
	
private:
	int id = 0;
	int groupId = 0;
};

bool UpdatToNewId()
{
	while(const Employee *employee = GetEmployee()) {
		if (!Valid(employee->GetId())) {
			return false;
		}
		Update(employee);
	}
	return true;
}
```

In this version, the `UpdateToNewId` function has been changed from a friend member function to a normal function. Its calls to `Employee::id` are accessed through the `Employee::GetId()` function and have no access to other `private` variables.

## Tip: implement the `Getter` function when defining the class in the header file

The `Getter` function is generally very small, so it can be directly defined in the header file of the class. The function implemented in the class definition has the inline keyword by default, which tells the compiler to consider the function inline (whether it is inline or not depends on the to the compiler).
