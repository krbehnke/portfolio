
/***  Compile with:  g++ -o orders.x orders.cpp  ***/

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;


// Problem 2:
// 
// A large part of our software is concerned with
// processing customer orders.  Part of processing these
// orders includes ensuring that they follow certain
// business rules.  We'd like to create an order validation
// class that allows teams across our departments to add
// rules in a decoupled manner.
// 
// Our order validation class should provide two main pieces
// of functionality:
// 1. Registering validators with the system.
// 2. Validating an order by:
//    2a. Applying validators to an order, AND
//    2b. Rejecting the order if any validator rejects the
//        order.
// 
// An order is composed of a list of items that the customer
// wants to buy, and the prices for each item.
// 
// You are expected to:
// 1. Design the order, order validation, and any other
//    classes/interfaces required.
// 2. You should also implement the order validation
//    class to provide registration and order rejection
//    capabilities as described above.
// 
// You are not required to implement any order validations
// yourself, but you should show how your classes and
// interfaces might be used by clients.
// 
// -----------------------
// 
// Solution:
// 
// For simplicity, each order will be a list of records each of
// which simply contains an item number and a price.  We assume
// that both items and orders may have any number of additional
// attributes.
//    Each validator will simply be a predicate which takes an
// order and returns true or false -- valid or not-valid.
//    "Order" will be a class.  After one or more calls to
// "add_item" are made, the function "process_order" will be
// invoked, which will in turn invoke each validator in the
// validator list.

class Order;

typedef bool (*validator_func)(const Order &order);

list <validator_func> validators;

void add_validator (const validator_func &func)
{
   validators.push_back (func);
}

class Item
{
public:
   int    itemno;
   double price;
}

class Order
{
public:
   void add_item (const Item &item) { order_list.push_back (item); }

   void process_order ();

   list <Item> order_list;
}

void Order::process_order ()
{
   list<validator_func>::const_iterator elt;

   for (elt = validators.begin();
        elt != validators.end();
        ++elt)
      if ( ! *elt (*this))
      {
         reject_order (*this);    // not implemented
         return;
      }

   finalize_order (*this);        // not implemented
}

