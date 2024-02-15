#include "ade.h"
#include "ade_gvars.h"



char *
str_duplicate (const char *src)
{
    size_t size = (strlen (src) + NULLTERM_PADDING) * sizeof (char);
    char *dup = malloc (size);
    assert (dup!= NULL);
    memcpy (dup, src, size);

    return dup;
}


char *
get_substring (const char *start, const char *end)
{
    char *sub;
    size_t size;

    assert (end >= start);

    size = end - start;
    sub = malloc ((size + NULLTERM_PADDING) * sizeof (char));
    assert (sub != NULL);
    memcpy (sub, start, size);
    sub[size] = '\0';

    return sub;
}


int
is_env (const char *raw_name)
{
    size_t prefix_size, suffix_size, min_size;
    size_t raw_size;

    char *env_ptr;
    size_t env_size;

    char *env;
    char *raw_value;

    prefix_size = strlen (ENV_PREFIX);
    suffix_size = strlen (ENV_SUFFIX);
    raw_size = strlen (raw_name);

    /* if the size of the passed string is smaller than min size, 
     * return false */
    min_size = prefix_size + suffix_size;
    if (raw_size <= min_size)
    {
        return 2;
    }

    /* get the enviornment variable's pointer and len */
    env_ptr = raw_name + prefix_size;
    env_size = raw_size - min_size;

    /* create a copy of the substring */
    env = malloc ((env_size + NULLTERM_PADDING) * sizeof (char));
    assert (env != NULL);
    memcpy (env, env_ptr, env_size);
    env[env_size] = '\0';
    
    /* get value associated with the env variable */
    raw_value = getenv (env);

    /* free the temporary substring */
    free (env);

    /* if the return is a NULL ptr then the variable does NOT exist */
    if (raw_value == NULL)
    {
        return 1;
    }

    /* otherwise, return true */
    return 0;
}


char *
replace_env (const char *raw_str)
{
    char *before_env_prefix, *before_env_suffix;
    char *after_env_prefix,  *after_env_suffix;

    char *leading, *trailing, *env;

    char *expanded_str;
    size_t size;
    char *raw_value, *value;

    /* get the prefix pointers */
    before_env_prefix = strstr (raw_str, ENV_PREFIX);
    if (before_env_prefix == NULL)
    {
        return NULL;
    }
    after_env_prefix = before_env_prefix + strlen (ENV_PREFIX);

    /* get suffix pointers */
    before_env_suffix = strstr (after_env_prefix, ENV_SUFFIX);
    if (before_env_suffix == NULL)
    {
        return NULL;
    }
    after_env_suffix = before_env_suffix + strlen (ENV_SUFFIX);

    /* create copies of substrings */
    leading  = get_substring (raw_str, before_env_prefix);
    env      = get_substring (after_env_prefix, before_env_suffix);
    trailing = str_duplicate (after_env_suffix);

    /* get value associated with the env variable */
    raw_value = getenv (env);
    if (raw_value != NULL)
    {
        value = raw_value;
    }
    else
    {
        value = "";
    }

    /* copy the substrings and env value into new string */
    size = strlen (leading) +
           strlen (value) +
           strlen (trailing);
    expanded_str = calloc (sizeof (char), size + NULLTERM_PADDING);
    strcat (expanded_str, leading);
    strcat (expanded_str, value);
    strcat (expanded_str, trailing);

    /* free the temporary strings */
    free (leading);
    free (env);
    free (trailing);

    /* retrurn the new string */
    return expanded_str;
}


char *
replace_env_i (const char *raw_str)
{
    char *expanded_str;
    char *head = str_duplicate (raw_str);

    while ((expanded_str = replace_env (head)) != NULL)
    {
        free (head);
        head = expanded_str;
    }

    return head;
}


char *
append_path (const char *orig_path, const char *appendee_item)
{
    char *appended_path = NULL;
    size_t new_size = 0;

    new_size = strlen (orig_path) +
               strlen (FOLDER_SEP) +
               strlen (appendee_item);

    appended_path = calloc (sizeof (char), new_size + NULLTERM_PADDING);
    assert (appended_path != NULL);

    strcat (appended_path, orig_path);
    strcat (appended_path, FOLDER_SEP);
    strcat (appended_path, appendee_item);

    return appended_path;
}


int
xdg_variables_exist (void)
{
    /* if any of the following variables are not set, 
     * don't use XDG envvars */
    if (is_env (XDG_CONFIG_ENV) != 0)
        return 1;

    if (is_env (XDG_CACHE_ENV) != 0)
        return 1;

    /* if all the previous are set, return success */
    return 0;
}


const char *
xdg_default (const char *xdg_env, const char *xdg_default)
{
    /* if enviornment variables are set, use them */
    if (xdg_variables_exist () == 0)
        return xdg_env;

    /* otherwise use a default value */
    return xdg_default;
}


void
set_global_directory_paths (void)
{
    char *xdg_config_home,      *xdg_cache_home;
    char *xdg_config_advantage, *xdg_cache_advantage; 

    char *home;
    char *home_advantage;


    /* expand variables when necessary to get absoulte paths */
    xdg_config_home = replace_env_i (xdg_default (XDG_CONFIG_ENV, XDG_CONFIG_DEFAULT));
    xdg_cache_home  = replace_env_i (xdg_default (XDG_CACHE_ENV,  XDG_CACHE_DEFAULT));

    /* use the XDG_CONFIG_HOME/ADE_CONF_DIR folder if it exists */
    xdg_config_advantage = append_path (xdg_config_home, ADE_CONF_DIR);
    xdg_cache_advantage  = append_path (xdg_cache_home, ADE_CONF_DIR);
  
    /* free the plain variable expansions */
    free (xdg_config_home);
    free (xdg_cache_home);

    /* if the XDG config directory has an advantage folder, 
     * use XDG locations */
    if (is_dir (xdg_config_advantage) == 0)
    {
        /* set the global directories */
        config_dir_path = xdg_config_advantage;
        cache_dir_path  = xdg_cache_advantage; 

        return;
    }

    /* free the xdg_advantage strings */
    free (xdg_config_advantage);
    free (xdg_cache_advantage);

    /* if XDG is not present, use the home folder paths */
    home = replace_env_i (HOME_ENV);
    home_advantage = append_path (home, ADE_CONF_DIR);
    free (home);

    config_dir_path = home_advantage;
    cache_dir_path = str_duplicate (home_advantage);

    return;
}


void
set_global_file_paths (void)
{
    set_global_directory_paths ();
    
    conf_file = append_path (config_dir_path, ADE_CONF_NAME);
    log_file = append_path (cache_dir_path, LOGFILENAME);
    screen_log_file = append_path (cache_dir_path, SCREENLOGFILENAME);
    
    return;
}


void
debug_print_xdg (void)
{
    printf ("config_dir_path: \"%s\"\n", config_dir_path);
    printf ("cache_dir_path: \"%s\"\n",  cache_dir_path);
    printf ("conf_file: \"%s\"\n",       conf_file);
    printf ("log_file: \"%s\"\n",        log_file);
    printf ("screen_log_file: \"%s\"\n", screen_log_file);
}


void
free_xdg_globals (void)
{
    free (config_dir_path);
    free (cache_dir_path);
    free (conf_file);
    free (log_file);
    free (screen_log_file);
}


