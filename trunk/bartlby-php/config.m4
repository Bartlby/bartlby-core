PHP_ARG_WITH(bartlby,whether to enable bartlby,
[  --enable-bartlby            Enable bartlby])

if test "$PHP_BARTLBY" != "no"; then
  
  PHP_EXTENSION(bartlby, $ext_shared)
fi
