#include "sai.h"
#include "stub_sai.h"
#include "assert.h"

#undef  __MODULE__
#define __MODULE__ SAI_LAG

#define MAX_NUMBER_OF_LAG_MEMBERS 16
#define MAX_NUMBER_OF_LAGS 5


static const sai_attribute_entry_t lag_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST, false, false, false, true,
        "List of ports in LAG", SAI_ATTR_VAL_TYPE_OBJLIST },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
        "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_attribute_entry_t lag_member_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID, true, true, false, true,
        "LAG ID", SAI_ATTR_VAL_TYPE_OID },
    { SAI_LAG_MEMBER_ATTR_PORT_ID, true, true, false, true,
        "PORT ID", SAI_ATTR_VAL_TYPE_OID },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
        "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};


sai_status_t get_lag_attribute(
        _In_ const sai_object_key_t *key,
        _Inout_ sai_attribute_value_t *value,
        _In_ uint32_t attr_index,
        _Inout_ vendor_cache_t *cache,
        void *arg);

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_attribute, (void*) SAI_LAG_ATTR_PORT_LIST,
      NULL, NULL }
};

sai_status_t get_lag_member_attribute(
        _In_ const sai_object_key_t *key,
        _Inout_ sai_attribute_value_t *value,
        _In_ uint32_t attr_index,
        _Inout_ vendor_cache_t *cache,
        void *arg);

static const sai_vendor_attribute_entry_t lag_member_vendor_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_LAG_ID,
      NULL, NULL },
    { SAI_LAG_MEMBER_ATTR_PORT_ID,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_PORT_ID,
      NULL, NULL }
};

typedef struct _lag_member_db_entry_t {
    bool            is_used;
    sai_object_id_t port_oid;
    sai_object_id_t lag_oid;
} lag_member_db_entry_t;

typedef struct _lag_db_entry_t {
    bool            is_used;
    sai_object_id_t members_ids[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db_entry_t;

struct lag_db_t {
    lag_db_entry_t        lags[MAX_NUMBER_OF_LAGS];
    lag_member_db_entry_t members[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db;


sai_status_t get_lag_attribute(
        _In_ const sai_object_key_t *key,
        _Inout_ sai_attribute_value_t *value,
        _In_ uint32_t attr_index,
        _Inout_ vendor_cache_t *cache,
        void *arg)
{
    sai_status_t status;
    sai_object_id_t member_oid;
    uint32_t db_index;
    uint32_t member_db_id;
    uint32_t port_count;

    assert((SAI_LAG_ATTR_PORT_LIST == (int64_t)arg));

    status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG, &db_index);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG DB index\n");
        return status;
    }

    switch ((int64_t)arg) {
        case SAI_LAG_ATTR_PORT_LIST:
            port_count = 0;
            for (int ii = 0; ii < MAX_NUMBER_OF_LAG_MEMBERS; ++ii) {
                if ((member_oid = lag_db.lags[db_index].members_ids[ii])) {
                    status = stub_object_to_type(member_oid, SAI_OBJECT_TYPE_LAG_MEMBER, &member_db_id);
                    if (status != SAI_STATUS_SUCCESS) {
                        printf("Cannot get LAG member DB index\n");
                        return status;
                    }
                    value->objlist.list[port_count] = lag_db.members[member_db_id].port_oid;
                    port_count++;
                }
            }
            value->objlist.count = port_count;
            break;
        default:
            printf("Got unexpected attribute ID\n");
            return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t get_lag_member_attribute(
        _In_ const sai_object_key_t *key,
        _Inout_ sai_attribute_value_t *value,
        _In_ uint32_t attr_index,
        _Inout_ vendor_cache_t *cache,
        void *arg)
{
    sai_status_t status;
    uint32_t db_index;

    assert((SAI_LAG_MEMBER_ATTR_LAG_ID == (int64_t)arg)
            || (SAI_LAG_MEMBER_ATTR_PORT_ID == (int64_t)arg));

    status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG_MEMBER, &db_index);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG member DB index\n");
        return status;
    }

    if (db_index >= MAX_NUMBER_OF_LAG_MEMBERS) {
        printf("Bad member OID\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (!lag_db.members[db_index].is_used) {
        printf("Member OID is not in use\n");
        return SAI_STATUS_ITEM_NOT_FOUND;
    }

    switch ((int64_t)arg) {
        case SAI_LAG_MEMBER_ATTR_LAG_ID:
            value->oid = lag_db.members[db_index].lag_oid;
            break;
        case SAI_LAG_MEMBER_ATTR_PORT_ID:
            value->oid = lag_db.members[db_index].port_oid;
            break;
        default:
            printf("Got unexpected attribute ID\n");
            return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}


sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    sai_status_t status = SAI_STATUS_SUCCESS;
    char list_str[MAX_LIST_VALUE_STR_LEN];
    STUB_LOG_ENTER();

    if (lag_id == NULL) {
        printf("Failed to create LAG (NULL ptr)\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    status = check_attribs_metadata(attr_count, attr_list, lag_attribs, lag_vendor_attribs, SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed attributes check for LAG create\n");
        return status;
    }

    uint32_t ii = 1;
    for (; ii < MAX_NUMBER_OF_LAGS; ++ii) {
        if (!lag_db.lags[ii].is_used) {
            break;
        }
    }
    if (ii == MAX_NUMBER_OF_LAGS) {
        printf("Cannot create LAG: limit is reached\n");
        return SAI_STATUS_FAILURE;
    }
    uint32_t lag_db_id = ii;
    status = stub_create_object(SAI_OBJECT_TYPE_LAG, lag_db_id, lag_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG OID\n");
        return status;
    }

    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("CREATE LAG: 0x%lX (%s)\n", *lag_id, list_str);

    lag_db.lags[lag_db_id].is_used = true;
    memset(lag_db.lags[lag_db_id].members_ids, 0, sizeof(lag_db.lags[lag_db_id].members_ids));

    STUB_LOG_EXIT();
    return status;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    sai_status_t status;
    STUB_LOG_ENTER();

    uint32_t     lag_db_id;
    status = stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG DB ID.\n");
        return status;
    }

    for (int ii = 0; ii < MAX_NUMBER_OF_LAG_MEMBERS; ++ii) {
        if (lag_db.lags[lag_db_id].members_ids[ii]) {
            printf("Cannot remove LAG, member list not empty\n");
            return SAI_STATUS_FAILURE;
        }
    }

    printf("REMOVE LAG: 0x%lX\n", lag_id);

    lag_db.lags[lag_db_id].is_used = false;
    memset(lag_db.lags[lag_db_id].members_ids, 0, sizeof(lag_db.lags[lag_db_id].members_ids));

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    STUB_LOG_ENTER();

    printf("SET LAG 0x%lX ATTRIBUTE\n", lag_id);

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t status;
    char list_str[MAX_LIST_VALUE_STR_LEN];
    STUB_LOG_ENTER();

    status = check_attribs_metadata(attr_count, attr_list, lag_attribs, lag_vendor_attribs, SAI_OPERATION_GET);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed attributes check for LAG attributes get\n");
        return status;
    }

    const sai_object_key_t key = { .object_id = lag_id };
    status = sai_get_attributes(&key, NULL, lag_attribs, lag_vendor_attribs, attr_count, attr_list);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get attributes\n");
        return status;
    }

    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("GET LAG 0x%lX ATTRIBUTE(S) (%s)\n", lag_id, list_str);

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count, _In_ sai_attribute_t *attr_list) {

    sai_status_t status = SAI_STATUS_SUCCESS;
    char list_str[MAX_LIST_VALUE_STR_LEN];
    STUB_LOG_ENTER();

    if (lag_member_id == NULL)
    {
        printf("Failed to create LAG member (NULL ptr)\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    status = check_attribs_metadata(attr_count, attr_list, lag_member_attribs, lag_member_vendor_attribs, SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed attributes check for LAG member create\n");
        return status;
    }

    uint32_t ii = 1;
    for (; ii < MAX_NUMBER_OF_LAG_MEMBERS; ++ii) {
        if (!lag_db.members[ii].is_used) {
            break;
        }
    }
    if (ii == MAX_NUMBER_OF_LAG_MEMBERS) {
        printf("Cannot create LAG member: limit is reached\n");
        return SAI_STATUS_FAILURE;
    }
    uint32_t lag_member_db_id = ii;
    status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, lag_member_db_id, lag_member_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create a LAG member OID\n");
        return status;
    }

    const sai_attribute_value_t *lag_id;
    uint32_t lag_id_ix;
    status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_LAG_ID, &lag_id, &lag_id_ix);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG member: no LAG ID provided\n");
        return status;
    }

    const sai_attribute_value_t *port_id;
    uint32_t port_id_ix;
    status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_PORT_ID, &port_id, &port_id_ix);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG member: no PORT ID provided\n");
        return status;
    }

    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("CREATE LAG MEMBER: 0x%lX (%s)\n", *lag_member_id, list_str);

    // update LAG entry to have this member
    uint32_t lag_db_id;
    status = stub_object_to_type(lag_id->oid, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG DB ID.\n");
        return status;
    }
    lag_db.lags[lag_db_id].members_ids[lag_member_db_id] = *lag_member_id;

    lag_db.members[lag_member_db_id].is_used = true;
    lag_db.members[lag_member_db_id].lag_oid = lag_id->oid;
    lag_db.members[lag_member_db_id].port_oid = port_id->oid;

    STUB_LOG_EXIT();
    return status;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
    sai_status_t status;
    uint32_t lag_db_id;
    uint32_t lag_member_db_id;
    STUB_LOG_ENTER();

    status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, &lag_member_db_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG member DB ID.\n");
        return status;
    }

    status = stub_object_to_type(lag_db.members[lag_member_db_id].lag_oid, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot get LAG DB ID.\n");
        return status;
    }

    printf("REMOVE LAG MEMBER: 0x%lX\n", lag_member_id);

    lag_db.members[lag_member_db_id].is_used = false;
    lag_db.members[lag_member_db_id].port_oid = 0;
    lag_db.members[lag_member_db_id].lag_oid = 0;

    // update LAG entry to remove this member
    lag_db.lags[lag_db_id].members_ids[lag_member_db_id] = 0;

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    STUB_LOG_ENTER();

    printf("SET LAG MEMBER 0x%lX ATTRIBUTE\n", lag_member_id);

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t status;
    char list_str[MAX_LIST_VALUE_STR_LEN];
    STUB_LOG_ENTER();

    status = check_attribs_metadata(attr_count, attr_list, lag_member_attribs, lag_member_vendor_attribs, SAI_OPERATION_GET);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed attributes check for LAG member attributes get\n");
        return status;
    }

    const sai_object_key_t key = { .object_id = lag_member_id };
    status = sai_get_attributes(&key, NULL, lag_member_attribs, lag_member_vendor_attribs, attr_count, attr_list);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get attributes\n");
        return status;
    }

    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("GET LAG MEMBER 0x%lX ATTRIBUTE(S) (%s)\n", lag_member_id, list_str);

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

const sai_lag_api_t lag_api = {
    stub_create_lag,
    stub_remove_lag,
    stub_set_lag_attribute,
    stub_get_lag_attribute,
    stub_create_lag_member,
    stub_remove_lag_member,
    stub_set_lag_member_attribute,
    stub_get_lag_member_attribute
};
