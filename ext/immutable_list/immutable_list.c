#include <stdio.h>
#include <ruby.h>

#define true 1
#define false 0

VALUE cImmutableList;

struct immutable_list {
  VALUE value;
  VALUE next;
};

static void
immutable_list_mark(struct immutable_list *ptr)
{
  rb_gc_mark(ptr->value);
  rb_gc_mark(ptr->next);
}

static VALUE
immutable_list_alloc(VALUE klass)
{
  struct immutable_list *ptr = ALLOC(struct immutable_list);
  return Data_Wrap_Struct(klass, immutable_list_mark, -1, ptr);
}

static VALUE
immutable_list_initialize(VALUE self)
{
  struct immutable_list *ptr;

  Data_Get_Struct(self, struct immutable_list, ptr);
  ptr->value = Qnil;
  ptr->next = Qnil;
  return self;
}

static VALUE
immutable_list_cons(VALUE self, VALUE a)
{
  struct immutable_list *ptr, *new_ptr;
  VALUE result;

  Data_Get_Struct(self, struct immutable_list, ptr);
  result = immutable_list_alloc(cImmutableList);
  Data_Get_Struct(result, struct immutable_list, new_ptr);

  new_ptr->value = a;
  new_ptr->next = self;
  return result;
}

static VALUE
immutable_list_s_create_core(VALUE result, int argc, VALUE *argv, int i)
{
  if (i < 0) {
    return result;
  } else {
    return immutable_list_s_create_core( immutable_list_cons(result, argv[i]), argc, argv, i-1);
  }
}

static VALUE
immutable_list_s_create(int argc, VALUE *argv, VALUE klass)
{
  VALUE result = immutable_list_initialize(immutable_list_alloc(klass));
  return immutable_list_s_create_core(result, argc, argv, argc - 1);
}

static VALUE
immutable_list_head(VALUE self)
{
  struct immutable_list *ptr;
  Data_Get_Struct(self, struct immutable_list, ptr);
  return ptr->value;
}

static VALUE
immutable_list_tail(VALUE self)
{
  struct immutable_list *ptr;
  Data_Get_Struct(self, struct immutable_list, ptr);
  return ptr->next;
}

static VALUE
immutable_list_is_empty(VALUE self)
{
  struct immutable_list *ptr;
  Data_Get_Struct(self, struct immutable_list, ptr);
  return ptr->next == Qnil ? Qtrue : Qfalse;
}

static VALUE
immutable_list_to_a(VALUE self)
{
  struct immutable_list *ptr;
  VALUE ary;
  
  ary = rb_ary_new();
  Data_Get_Struct(self, struct immutable_list, ptr);
  while (ptr->next != Qnil) {
    rb_ary_push(ary, ptr->value);
    Data_Get_Struct(ptr->next, struct immutable_list, ptr);
  }
  return ary;
}

static VALUE
immutable_list_inspect(VALUE self)
{
  VALUE str, str2;
  struct immutable_list *ptr;

  Data_Get_Struct(self, struct immutable_list, ptr);
  str = rb_str_buf_new2("(");

  if (ptr->next != Qnil) {
    while (true) {
      str2 = rb_inspect(ptr->value);
      rb_str_buf_append(str, str2);
      Data_Get_Struct(ptr->next, struct immutable_list, ptr);
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
immutable_list_rev_append(VALUE l1, VALUE l2)
{
  struct immutable_list *ptr;

  Data_Get_Struct(l1, struct immutable_list, ptr);
  if (ptr->next == Qnil) {
    return l2;
  } else {
    return immutable_list_rev_append(ptr->next, immutable_list_cons(l2, ptr->value));
  }
}

static VALUE
immutable_list_rev(VALUE self)
{
  return immutable_list_rev_append(self,
      immutable_list_initialize(immutable_list_alloc(cImmutableList)));
}

static VALUE
immutable_list_append(VALUE l1, VALUE l2)
{
  struct immutable_list *ptr;

  Data_Get_Struct(l1, struct immutable_list, ptr);
  if (ptr->next == Qnil) {
    return l2;
  } else {
    return immutable_list_cons( immutable_list_append(ptr->next, l2), ptr->value);
  }
}

static int
immutable_list_length_core(VALUE self)
{
  struct immutable_list *ptr;

  Data_Get_Struct(self, struct immutable_list, ptr);
  if (ptr->next == Qnil) {
    return 0;
  } else {
    return 1 + immutable_list_length_core(ptr->next);
  }
}

static VALUE
immutable_list_length(VALUE self)
{
  return INT2FIX(immutable_list_length_core(self));
}

static VALUE
immutable_list_nth(VALUE self, VALUE index)
{
  struct immutable_list *ptr;
  int i, n;
  
  n = FIX2INT(index);
  Data_Get_Struct(self, struct immutable_list, ptr);
  for (i = 0; ptr->next != Qnil; ++i) {
    if (i == n) {
      return ptr->value;
    }
    Data_Get_Struct(ptr->next, struct immutable_list, ptr);
  }
  return Qnil;
}

void Init_immutable_list(void)
{
  cImmutableList = rb_define_class("ImmutableList", rb_cObject);
  rb_define_alloc_func(cImmutableList, immutable_list_alloc);
  rb_define_singleton_method(cImmutableList, "[]", immutable_list_s_create, -1);

  rb_define_private_method(cImmutableList, "initialize", immutable_list_initialize, 0);
  rb_define_method(cImmutableList, "cons", immutable_list_cons, 1);
  rb_define_method(cImmutableList, "head", immutable_list_head, 0);
  rb_define_method(cImmutableList, "tail", immutable_list_tail, 0);
  rb_define_method(cImmutableList, "empty?", immutable_list_is_empty, 0);
  rb_define_method(cImmutableList, "to_a", immutable_list_to_a, 0);
  rb_define_method(cImmutableList, "inspect", immutable_list_inspect, 0);
  rb_define_alias(cImmutableList,  "to_s", "inspect");
  rb_define_method(cImmutableList, "rev_append", immutable_list_rev_append, 1);
  rb_define_method(cImmutableList, "rev", immutable_list_rev, 0);
  rb_define_method(cImmutableList, "append", immutable_list_append, 1);
  rb_define_alias(cImmutableList,  "+", "append");
  rb_define_method(cImmutableList, "length", immutable_list_length, 0);
  rb_define_method(cImmutableList, "nth", immutable_list_nth, 1);
  rb_define_alias(cImmutableList,  "[]", "nth");
}
