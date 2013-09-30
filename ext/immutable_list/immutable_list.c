#include <stdio.h>
#include <ruby.h>

#define true 1
#define false 0

VALUE cLinkedList;

struct linkedlist {
  VALUE value;
  VALUE next;
};

static void
linkedlist_mark(struct linkedlist *ptr)
{
  rb_gc_mark(ptr->value);
  rb_gc_mark(ptr->next);
}

static VALUE
linkedlist_alloc(VALUE klass)
{
  struct linkedlist *ptr = ALLOC(struct linkedlist);
  return Data_Wrap_Struct(klass, linkedlist_mark, -1, ptr);
}

static VALUE
linkedlist_initialize(VALUE self)
{
  struct linkedlist *ptr;

  Data_Get_Struct(self, struct linkedlist, ptr);
  ptr->value = Qnil;
  ptr->next = Qnil;
  return self;
}

static VALUE
linkedlist_cons(VALUE self, VALUE a)
{
  struct linkedlist *ptr, *new_ptr;
  VALUE result;

  Data_Get_Struct(self, struct linkedlist, ptr);
  result = linkedlist_alloc(cLinkedList);
  Data_Get_Struct(result, struct linkedlist, new_ptr);

  new_ptr->value = a;
  new_ptr->next = self;
  return result;
}

static VALUE
linkedlist_s_create_core(VALUE result, int argc, VALUE *argv, int i)
{
  if (i < 0) {
    return result;
  } else {
    return linkedlist_s_create_core( linkedlist_cons(result, argv[i]), argc, argv, i-1);
  }
}

static VALUE
linkedlist_s_create(int argc, VALUE *argv, VALUE klass)
{
  VALUE result = linkedlist_initialize(linkedlist_alloc(klass));
  return linkedlist_s_create_core(result, argc, argv, argc - 1);
}

static VALUE
linkedlist_head(VALUE self)
{
  struct linkedlist *ptr;
  Data_Get_Struct(self, struct linkedlist, ptr);
  return ptr->value;
}

static VALUE
linkedlist_tail(VALUE self)
{
  struct linkedlist *ptr;
  Data_Get_Struct(self, struct linkedlist, ptr);
  return ptr->next;
}

static VALUE
linkedlist_is_empty(VALUE self)
{
  struct linkedlist *ptr;
  Data_Get_Struct(self, struct linkedlist, ptr);
  return ptr->next == Qnil ? Qtrue : Qfalse;
}

static VALUE
linkedlist_to_a(VALUE self)
{
  struct linkedlist *ptr;
  VALUE ary;
  
  ary = rb_ary_new();
  Data_Get_Struct(self, struct linkedlist, ptr);
  while (ptr->next != Qnil) {
    rb_ary_push(ary, ptr->value);
    Data_Get_Struct(ptr->next, struct linkedlist, ptr);
  }
  return ary;
}

static VALUE
linkedlist_inspect(VALUE self)
{
  VALUE str, str2;
  struct linkedlist *ptr;

  Data_Get_Struct(self, struct linkedlist, ptr);
  str = rb_str_buf_new2("(");

  if (ptr->next != Qnil) {
    while (true) {
      str2 = rb_inspect(ptr->value);
      rb_str_buf_append(str, str2);
      Data_Get_Struct(ptr->next, struct linkedlist, ptr);
      if (ptr->next != Qnil) {
        rb_str_buf_cat_ascii(str, ", ");
      } else {
        break;
      }
    }
  }

  rb_str_buf_cat2(str, ")");
  OBJ_INFECT(str, self);

  return str;
}

static VALUE
linkedlist_rev_append(VALUE l1, VALUE l2)
{
  struct linkedlist *ptr;

  Data_Get_Struct(l1, struct linkedlist, ptr);
  if (ptr->next == Qnil) {
    return l2;
  } else {
    return linkedlist_rev_append(ptr->next, linkedlist_cons(l2, ptr->value));
  }
}

static VALUE
linkedlist_rev(VALUE self)
{
  return linkedlist_rev_append(self,
      linkedlist_initialize(linkedlist_alloc(cLinkedList)));
}

static VALUE
linkedlist_append(VALUE l1, VALUE l2)
{
  struct linkedlist *ptr;

  Data_Get_Struct(l1, struct linkedlist, ptr);
  if (ptr->next == Qnil) {
    return l2;
  } else {
    return linkedlist_cons( linkedlist_append(ptr->next, l2), ptr->value);
  }
}

static int
linkedlist_length_core(VALUE self)
{
  struct linkedlist *ptr;

  Data_Get_Struct(self, struct linkedlist, ptr);
  if (ptr->next == Qnil) {
    return 0;
  } else {
    return 1 + linkedlist_length_core(ptr->next);
  }
}

static VALUE
linkedlist_length(VALUE self)
{
  return INT2FIX(linkedlist_length_core(self));
}

static VALUE
linkedlist_nth(VALUE self, VALUE index)
{
  struct linkedlist *ptr;
  int i, n;
  
  n = FIX2INT(index);
  Data_Get_Struct(self, struct linkedlist, ptr);
  for (i = 0; ptr->next != Qnil; ++i) {
    if (i == n) {
      return ptr->value;
    }
    Data_Get_Struct(ptr->next, struct linkedlist, ptr);
  }
  return Qnil;
}

void Init_linkedlist(void)
{
  cLinkedList = rb_define_class("ImmutableList", rb_cObject);
  rb_define_alloc_func(cLinkedList, linkedlist_alloc);
  rb_define_singleton_method(cLinkedList, "[]", linkedlist_s_create, -1);

  rb_define_private_method(cLinkedList, "initialize", linkedlist_initialize, 0);
  rb_define_method(cLinkedList, "cons", linkedlist_cons, 1);
  rb_define_method(cLinkedList, "head", linkedlist_head, 0);
  rb_define_method(cLinkedList, "tail", linkedlist_tail, 0);
  rb_define_method(cLinkedList, "empty?", linkedlist_is_empty, 0);
  rb_define_method(cLinkedList, "to_a", linkedlist_to_a, 0);
  rb_define_method(cLinkedList, "inspect", linkedlist_inspect, 0);
  rb_define_alias(cLinkedList,  "to_s", "inspect");
  rb_define_method(cLinkedList, "rev_append", linkedlist_rev_append, 1);
  rb_define_method(cLinkedList, "rev", linkedlist_rev, 0);
  rb_define_method(cLinkedList, "append", linkedlist_append, 1);
  rb_define_alias(cLinkedList,  "+", "append");
  rb_define_method(cLinkedList, "length", linkedlist_length, 0);
  rb_define_method(cLinkedList, "nth", linkedlist_nth, 1);
  rb_define_alias(cLinkedList,  "[]", "nth");
}
