#!/usr/bin/env python3

import argparse
import re
import os.path
import sys
import textwrap
import yaml

h_template = '''\
#pragma once

#include <glib-object.h>

#include "cog/cog-macros.h"
{h_file_head}\

G_BEGIN_DECLS

#define COG_TYPE_{screaming_snake} (cog_{snake}_get_type ())

typedef struct _Cog{camel} Cog{camel};

/**
 * Cog{camel}:
{field_doc}
 *
{doc}
 */
struct _Cog{camel}
{{
{fields}

  /*< private >*/
  unsigned ref_count;
}};

COG_AVAILABLE_IN_ALL
GType cog_{snake}_get_type (void) G_GNUC_CONST;

{constructor_decl}
{field_setter_decls}
COG_AVAILABLE_IN_ALL
Cog{camel} *cog_{snake}_copy (Cog{camel} *self);

COG_AVAILABLE_IN_ALL
Cog{camel} *cog_{snake}_ref (Cog{camel} *self);

COG_AVAILABLE_IN_ALL
void cog_{snake}_unref (Cog{camel} *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Cog{camel}, cog_{snake}_unref)

G_END_DECLS
'''

h_constructor_template = '''\
COG_AVAILABLE_IN_ALL
Cog{camel} *cog_{snake}_new (void);
'''

h_field_setter_template = '''\
COG_AVAILABLE_IN_ALL
void cog_{snake}_set_{field_name} (Cog{camel} *self,
{space____________}                {field_setter_type_in}{field_name});\
'''

c_template = '''\
#include <aws/cognito-idp/model/{camel}Type.h>

#include "cog/cog-{kebab}.h"
{c_file_head}\

using Aws::CognitoIdentityProvider::Model::{camel}Type;

G_DEFINE_BOXED_TYPE (Cog{camel}, cog_{snake},
                     cog_{snake}_ref, cog_{snake}_unref)

{constructor}
{field_setters}
/**
 * cog_{snake}_copy:
 * @self: a #Cog{camel}
 *
 * Makes a deep copy of a #Cog{camel}.
 *
 * Returns: (transfer full): A #Cog{camel} with the same contents
 *  as @self
 */
Cog{camel} *
cog_{snake}_copy (Cog{camel} *self)
{{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  Cog{camel} *copy = {underscore}cog_{snake}_new ();
{fields_copy}

  return copy;
}}

static void
cog_{snake}_free (Cog{camel} *self)
{{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

{fields_free}

  g_slice_free (Cog{camel}, self);
}}

/**
 * cog_{snake}_ref:
 * @self: A #Cog{camel}
 *
 * Increments the reference count of @self by one.
 *
 * Returns: (transfer none): @self
 */
Cog{camel} *
cog_{snake}_ref (Cog{camel} *self)
{{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}}

/**
 * cog_{snake}_unref:
 * @self: (transfer none): A #Cog{camel}
 *
 * Decrements the reference count of @self by one, freeing the structure when
 * the reference count reaches zero.
 */
void
cog_{snake}_unref (Cog{camel} *self)
{{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    cog_{snake}_free (self);
}}
{from_internal}\
{to_internal}\
'''

c_constructor_template = '''\
{doc}\
{static}Cog{camel} *
{underscore}cog_{snake}_new (void)
{{
  Cog{camel} *self = g_slice_new0 (Cog{camel});
  self->ref_count = 1;

  return self;
}}
'''

c_constructor_doc_template = '''\
/**
 * cog_{snake}_new:
 *
 * Creates a new #Cog{camel}.
 *
 * Returns: (transfer full): A newly created #Cog{camel}
 */
'''

c_field_setter_template = '''\
/**
 * cog_{snake}_set_{field_name}:
 * @self: the #Cog{camel}
 * @{field_name}{annotations}: a {field_type}
 *
 * See #Cog{camel}.{field_name}.
 */
void
cog_{snake}_set_{field_name} (Cog{camel} *self,
{space____________}           {field_setter_type_in}{field_name})
{{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);
  {validate_field_argument}

  {free_existing_field}
{set_field}
}}
'''

c_from_internal_template = '''
Cog{camel} *
_cog_{snake}_from_internal (const {camel}Type& internal)
{{
  Cog{camel} *retval = _cog_{snake}_new ();
{marshal_fields}
  return retval;
}}
'''

c_to_internal_template = '''\
{camel}Type
_cog_{snake}_to_internal (Cog{camel} *self)
{{
  return {camel}Type ()
{with_fields}
    ;
}}
'''


def snakeify(camel):
    # https://stackoverflow.com/a/1176023
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', camel)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def kebabify(snake):
    return re.sub('_', '-', snake)


def is_pod(field_type):
    """Returns whether a type is POD (Plain Old Data)"""
    return field_type in ('bool', 'integer', 'enum')


def field_decl_type(field):
    """Return the struct declaration type for a field"""
    field_type = field['type']
    if field_type == 'string':
        return 'char *'
    if field_type == 'integer':
        return 'int '
    if field_type == 'object':
        return 'Cog{} *'.format(field['class'])
    if field_type == 'enum':
        return 'Cog{} '.format(field['class'])
    raise ValueError('add a field decl type for {}'.format(field_type))


def field_setter_type_in(field_type):
    """Return the type for the second parameter of a field setter"""
    field_type = field['type']
    if field_type == 'string':
        return 'const char *'
    if field_type == 'integer':
        return 'int '
    if field_type == 'object':
        return 'Cog{} *'.format(field['class'])
    if field_type == 'enum':
        return 'Cog{} '.format(field['class'])
    raise ValueError('add a field setter type for {}'.format(field_type))


def validate_field_argument(field):
    """Return an expression to validate an argument of type @field, or None if
    not needed"""
    field_type = field['type']
    is_nullable = 'nullable' in field.get('annotations', [])
    if field_type == 'string' and is_nullable:
        return '!{0} || *{0}'.format(snakeify(field['name']))
    if is_pod(field_type):
        return None
    if field_type == 'object' and is_nullable:
        return None
    raise ValueError('add a validation template for {}'.format(field_type))


def free_existing_field(field):
    """Return code to free any existing value in the field, or None if not
    needed"""
    field_type = field['type']
    snake_name = snakeify(field['name'])
    if field_type == 'string':
        return 'g_clear_pointer (&self->{}, g_free);'.format(snake_name)
    if is_pod(field_type):
        return None
    if field_type == 'object':
        return 'g_clear_pointer (&self->{}, cog_{}_unref);'.format(
            snake_name, snakeify(field['class']))
    raise ValueError('add a free template for {}'.format(field_type))


def set_field(field):
    """Return code to set a new value in the field"""
    field_type = field['type']
    is_nullable = 'nullable' in field.get('annotations', [])
    snake_name = snakeify(field['name'])
    if field_type == 'string' and is_nullable:
        return textwrap.dedent('''\
            if ({0})
              self->{0} = g_strdup ({0});'''.format(snake_name))
    if is_pod(field_type):
        return 'self->{0} = {0};'.format(snake_name)
    if field['type'] == 'object' and is_nullable:
        return textwrap.dedent('''\
            if ({0})
              self->{0} = cog_{1}_copy ({0});'''.format(
            snake_name, snakeify(field['class'])))
    raise ValueError('add a set template for {}'.format(field_type))


def copy_field(field):
    """Return code to deep-copy a field"""
    field_type = field['type']
    is_nullable = 'nullable' in field.get('annotations', [])
    snake_name = snakeify(field['name'])
    if field_type == 'string' and is_nullable:
        return textwrap.dedent('''\
            if (self->{0})
              copy->{0} = g_strdup (self->{0});'''.format(snake_name))
    if is_pod(field_type):
        return 'copy->{0} = self->{0};'.format(snake_name)
    if field_type == 'object' and is_nullable:
        return textwrap.dedent('''\
            if (self->{0})
              copy->{0} = cog_{1}_copy (self->{0});'''.format(
            snake_name, snakeify(field['class'])))
    raise ValueError('add a copy template for {}'.format(field_type))


def marshal_field(field):
    """Return code to marshal a field from the AWS struct"""
    field_type = field['type']
    if field_type == 'string':
        return 'g_strdup (internal.Get{[name]} ().c_str ())'.format(field)
    if field_type == 'enum':
        return 'Cog{0[name]} (internal.Get{0[name]} ())'.format(field)
    if is_pod(field_type):
        return 'internal.Get{[name]} ()'.format(field)
    if field_type == 'object':
        return '_cog_{}_from_internal (internal.Get{[name]} ())'.format(
            snakeify(field['class']), field)
    raise ValueError('add a marshal template for {}'.format(field_type))


def wrap_gtkdoc(text):
    return textwrap.wrap(text, width=80, initial_indent=' * ',
                         subsequent_indent=' * ')


parser = argparse.ArgumentParser(description='Generate boxed types from AWS')
parser.add_argument('infile', type=open, default=sys.stdin)
parser.add_argument('outdir', nargs='?', default='.')
args = parser.parse_args()

schema = yaml.load(args.infile)

camel = schema['type']
snake = snakeify(camel)
kebab = kebabify(snake)
screaming_snake = snake.upper()
name_formats = dict(camel=camel, snake=snake, kebab=kebab,
                    screaming_snake=screaming_snake)

h_outfile_name = os.path.join(args.outdir, 'cog-' + kebab + '.h')
c_outfile_name = os.path.join(args.outdir, 'cog-' + kebab + '.cpp')

field_docs = []
fields = []
field_setter_decls = []
field_setters = []
field_copy_code = []
field_free_code = []
with_fields = []
marshal_fields = []
for field in schema['fields']:
    decl_type = field_decl_type(field)
    field_camel = field['name']
    field_snake = snakeify(field_camel)
    free_code = free_existing_field(field)

    doc = '@{}: {}'.format(field_snake, field['doc'])
    field_docs += wrap_gtkdoc(doc)

    fields += ['  {}{};'.format(decl_type, field_snake)]

    if field.get('setter', False):
        space = ' ' * (len(snake) + len(field_snake))
        field_setter_decls += [h_field_setter_template.format(
            **name_formats, field_name=field_snake,
            field_setter_type_in=field_setter_type_in(field),
            space____________=space)]

        validate_expr = validate_field_argument(field)
        validate_field_code = ''
        if validate_expr:
            validate_field_code = 'g_return_if_fail ({});'.format(
                validate_expr)
        annotations = ''
        if field.get('annotations', None):
            annotations = ': (' + ') ('.join(field['annotations']) + ')'
        field_setters += [c_field_setter_template.format(
            **name_formats, field_name=field_snake, field_type=field['type'],
            field_setter_type_in=field_setter_type_in(field),
            validate_field_argument=validate_field_code,
            free_existing_field=free_code or '',
            set_field=textwrap.indent(set_field(field), '  '),
            annotations=annotations, space____________=space)]

    field_copy_code += [textwrap.indent(copy_field(field), '  ')]
    if free_code:
        field_free_code += [textwrap.indent(free_code, '  ')]
    with_fields += ['    .With{} (self->{})'.format(field_camel, field_snake)]
    marshal_fields += ['  retval->{} = {};'.format(
        field_snake, marshal_field(field))]

from_internal = ''
if schema.get('from_internal', False):
    from_internal = c_from_internal_template.format(
        **name_formats, marshal_fields='\n'.join(marshal_fields))

to_internal = ''
if schema.get('to_internal', False):
    to_internal = c_to_internal_template.format(
        **name_formats, with_fields='\n'.join(with_fields))

private_constructor = schema.get('from_internal', False)
constructor_decl = ''
constructor_doc = ''
underscore, static = '_', 'static '
if not private_constructor:
    constructor_decl = h_constructor_template.format(**name_formats)
    constructor_doc = c_constructor_doc_template.format(**name_formats)
    underscore, static = '', ''

constructor = c_constructor_template.format(
    **name_formats, static=static, underscore=underscore,
    doc=constructor_doc)

h_contents = h_template.format(
    **name_formats, h_file_head=schema.get('h_file_head', '\n'),
    doc='\n'.join(wrap_gtkdoc(schema['doc'])),
    fields='\n'.join(fields), field_doc='\n'.join(field_docs),
    constructor_decl=constructor_decl,
    field_setter_decls='\n\n'.join(field_setter_decls))

c_contents = c_template.format(
    **name_formats, c_file_head=schema.get('c_file_head', '\n'),
    constructor=constructor, underscore=underscore,
    field_setters='\n\n'.join(field_setters),
    fields_copy='\n'.join(field_copy_code),
    fields_free='\n'.join(field_free_code), from_internal=from_internal,
    to_internal=to_internal)

with open(h_outfile_name, 'w') as f:
    f.write(h_contents)

with open(c_outfile_name, 'w') as f:
    f.write(c_contents)
