// Copyright 2018 Michael Tautschnig

/// \file Tests that irept memory consumption is fixed

#include <testing-utils/catch.hpp>
#include <util/irep.h>

SCENARIO("irept_memory", "[core][utils][irept]")
{
  GIVEN("Always")
  {
    THEN("An irept is just a pointer")
    {
      REQUIRE(sizeof(irept) == sizeof(void *));
    }

    THEN("get_nil_irep yields ID_nil")
    {
      REQUIRE(get_nil_irep().id() == ID_nil);
      REQUIRE(get_nil_irep().is_nil());
      REQUIRE(!get_nil_irep().is_not_nil());
    }
  }

  GIVEN("An initialized irep")
  {
    irept irep("some_id");
    irept irep_copy(irep);
    irept irep_assign = irep;

    irept irep_other("some_other_id");

    THEN("Its id is some_id")
    {
      REQUIRE(irep.id() == "some_id");
      REQUIRE(irep_copy.id() == "some_id");
      REQUIRE(irep_assign.id() == "some_id");

      REQUIRE(irep_other.id() == "some_other_id");

      // TODO(tautschnig): id_string() should be deprecated in favour of
      // id2string(...)
      REQUIRE(irep.id_string().size() == 7);
    }

    THEN("Swapping works")
    {
      irep.swap(irep_other);

      REQUIRE(irep.id() == "some_other_id");
      REQUIRE(irep_copy.id() == "some_id");
      REQUIRE(irep_assign.id() == "some_id");

      REQUIRE(irep_other.id() == "some_id");
    }
  }

  GIVEN("An irep")
  {
    irept irep;

    THEN("Its id is empty")
    {
      REQUIRE(irep.is_not_nil());
      REQUIRE(irep.id().empty());
    }

    THEN("Its id can be set")
    {
      irep.id("new_id");
      REQUIRE(irep.id() == "new_id");
    }

    THEN("find of a non-existent element yields nil")
    {
      REQUIRE(irep.find("no-such-element").is_nil());
    }

    THEN("Adding/removing elements is possible")
    {
      REQUIRE(irep.get_sub().empty());
      REQUIRE(irep.get_named_sub().empty());
      REQUIRE(irep.get_comments().empty());

      irept &e = irep.add("a_new_element");
      REQUIRE(e.id().empty());
      e.id("some_id");
      REQUIRE(irep.find("a_new_element").id() == "some_id");

      irept irep2("second_irep");
      irep.add("a_new_element", irep2);
      REQUIRE(irep.find("a_new_element").id() == "second_irep");
      REQUIRE(irep.get_named_sub().size() == 1);

      irep.add("#a_comment", irep2);
      REQUIRE(irep.find("#a_comment").id() == "second_irep");
      REQUIRE(irep.get_comments().size() == 1);

      irept bak(irep);
      irep.remove("no_such_id");
      REQUIRE(bak == irep);
      irep.remove("a_new_element");
      REQUIRE(bak != irep);
      REQUIRE(irep.find("a_new_element").is_nil());

      irep.move_to_sub(bak);
      REQUIRE(irep.get_sub().size() == 1);

      irep.move_to_named_sub("another_entry", irep2);
      REQUIRE(irep.get_sub().size() == 1);
      REQUIRE(irep.get_named_sub().size() == 1);
      REQUIRE(irep.get_comments().size() == 1);

      irept irep3;
      irep.move_to_named_sub("#a_comment", irep3);
      REQUIRE(irep.find("#a_comment").id().empty());
      REQUIRE(irep.get_sub().size() == 1);
      REQUIRE(irep.get_named_sub().size() == 1);
      REQUIRE(irep.get_comments().size() == 1);

      irept irep4;
      irep.move_to_named_sub("#another_comment", irep4);
      REQUIRE(irep.get_comments().size() == 2);
    }

    THEN("Setting and getting works")
    {
      // TODO(tautschnig): get_string() should be deprecated in favour of
      // id2string(...)
      REQUIRE(irep.get_string("no_such_id").empty());

      REQUIRE(irep.get("no_such_id").empty());
      // TODO(tautschnig): it's not clear whether we need all of the below
      // variants in the API
      REQUIRE(!irep.get_bool("no_such_id"));
      REQUIRE(irep.get_int("no_such_id") == 0);
      REQUIRE(irep.get_unsigned_int("no_such_id") == 0u);
      REQUIRE(irep.get_size_t("no_such_id") == 0u);
      REQUIRE(irep.get_long_long("no_such_id") == 0);

      irep.set("some_id", "some string");
      REQUIRE(irep.get("some_id") == "some string");
      irept irep2("second_irep");
      irep.set("a_new_element", irep2);
      REQUIRE(irep.find("a_new_element").id() == "second_irep");
      irep.set("numeric_id", 1);
      REQUIRE(irep.get_bool("numeric_id"));
      irep.set("numeric_id", 42);
      REQUIRE(!irep.get_bool("numeric_id"));
      REQUIRE(irep.get_int("numeric_id") == 42);
      REQUIRE(irep.get_unsigned_int("numeric_id") == 42u);
      REQUIRE(irep.get_size_t("numeric_id") == 42u);
      REQUIRE(irep.get_long_long("numeric_id") == 42);

      irep.clear();
      REQUIRE(irep.id().empty());
      REQUIRE(irep.get_sub().empty());
      REQUIRE(irep.get_named_sub().empty());
      REQUIRE(irep.get_comments().empty());

      irep.make_nil();
      REQUIRE(irep.id() == ID_nil);
      REQUIRE(irep.get_sub().empty());
      REQUIRE(irep.get_named_sub().empty());
      REQUIRE(irep.get_comments().empty());
    }

    THEN("Pretty printing works")
    {
      irept sub("sub_id");

      irep.id("our_id");
      irep.add("some_op", sub);
      irep.add("#comment", sub);
      irep.move_to_sub(sub);

      std::string pretty = irep.pretty();
      REQUIRE(
        pretty ==
        "our_id\n"
        "  * some_op: sub_id\n"
        "  * #comment: sub_id\n"
        "  0: sub_id");
    }

    THEN("Hashing works")
    {
      irep.id("some_id");
      irep.set("#a_comment", 42);

      REQUIRE(irep.hash() != 0);
      REQUIRE(irep.full_hash() != 0);
      REQUIRE(irep.hash() != irep.full_hash());
    }
  }

  GIVEN("Multiple ireps")
  {
    irept irep1, irep2;

    THEN("Comparison works")
    {
      REQUIRE(irep1 == irep2);
      REQUIRE(irep1.full_eq(irep2));

      irep1.id("id1");
      irep2.id("id2");
      REQUIRE(irep1 != irep2);
      const bool one_lt_two = irep1 < irep2;
      const bool two_lt_one = irep2 < irep1;
      REQUIRE(one_lt_two != two_lt_one);
      REQUIRE(irep1.ordering(irep2) != irep2.ordering(irep1));
      REQUIRE(irep1.compare(irep2) != 0);

      irep2.id("id1");
      REQUIRE(irep1 == irep2);
      REQUIRE(irep1.full_eq(irep2));

      irep2.set("#a_comment", 42);
      REQUIRE(irep1 == irep2);
      REQUIRE(!irep1.full_eq(irep2));
    }
  }
}
