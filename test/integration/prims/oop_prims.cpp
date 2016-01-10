#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_oop_prims.cpp.incl"


class OopPrimitivesBecomeTest : public ::testing::Test {
protected:
  objArrayOop targetContainer, replacementContainer, filler;
  associationOop tenuredTargetContainer, tenuredReplacementContainer;
  memOop target, replacement;
  oop saveNil;

virtual void SetUp() {
  klassOop objectClass = klassOop(Universe::find_global("Object"));

  target = memOop(objectClass->primitive_allocate());
  replacement = memOop(objectClass->primitive_allocate());

  targetContainer = oopFactory::new_objArray(1);
  filler = oopFactory::new_objArray(128); // make sure containers are on different cards
  replacementContainer = oopFactory::new_objArray(1);

  targetContainer->obj_at_put(1, target);
  replacementContainer->obj_at_put(1, replacement);

  tenuredTargetContainer = oopFactory::new_association(vmSymbols::value(), target, false);
  tenuredReplacementContainer = oopFactory::new_association(vmSymbols::value(), replacement, false);

  assert(Universe::old_gen.contains(tenuredTargetContainer), "target container should be tenured");
  assert(Universe::old_gen.contains(tenuredReplacementContainer), "replacement container should be tenured");

  saveNil = Universe::nilObj();
}

virtual void TearDown(){
  targetContainer = replacementContainer = NULL;
  tenuredTargetContainer = tenuredReplacementContainer = NULL;
  target = replacement = NULL;
  if (Universe::nilObj() != saveNil)
    oopPrimitives::become(saveNil, Universe::nilObj());
  MarkSweep::collect();
}
};

TEST_F(OopPrimitivesBecomeTest, becomeShouldSwapTargetAndReplacement)
{
  oopPrimitives::become(replacement, target);
  ASSERT_EQ(replacement, targetContainer->obj_at(1)) << "target of become: has not been replaced by replacement";
  ASSERT_EQ(target, replacementContainer->obj_at(1)) << "replacement has not been replaced by target of become:";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldReturnTarget)
{
  ASSERT_EQ(target, oopPrimitives::become(replacement, target)) << "should return target";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldMarkStoredCards)
{
  Universe::remembered_set->clear();

  oopPrimitives::become(replacement, target);

  ASSERT_TRUE(Universe::remembered_set->is_dirty(targetContainer)) << "target container should be diry";
  ASSERT_TRUE(Universe::remembered_set->is_dirty(replacementContainer)) << "replacement container should be diry";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldSwapTargetAndReplacementReferencesInTenuredObjects)
{
  oopPrimitives::become(replacement, target);
  ASSERT_EQ(replacement, tenuredTargetContainer->value()) << "target of become: has not been replaced by replacement";
  ASSERT_EQ(target, tenuredReplacementContainer->value()) << "replacement has not been replaced by target of become:";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldUpdateRoots)
{
  oopPrimitives::become(Universe::nilObj(), target);
  ASSERT_EQ(target, Universe::nilObj()) << "nilObj should now be target";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldReturnErrorWhenReceiverIsSmallInteger)
{
  ASSERT_EQ(markSymbol(vmSymbols::first_argument_has_wrong_type()),
    oopPrimitives::become(replacement, smiOop_one)) << "receiver cannot be small integer";
}

TEST_F(OopPrimitivesBecomeTest, becomeShouldReturnErrorWhenReplacementIsSmallInteger)
{
  ASSERT_EQ(markSymbol(vmSymbols::second_argument_has_wrong_type()),
    oopPrimitives::become(smiOop_one, replacement)) << "replacement cannot be small integer";
}
