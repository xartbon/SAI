#include "sai.h"
#include "stub_sai.h"
#include "assert.h"

#undef  __MODULE__
#define __MODULE__ SAI_LAG

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    static int32_t next_lag_id = 1;
    sai_status_t status = SAI_STATUS_SUCCESS;
    STUB_LOG_ENTER();

    status = stub_create_object(SAI_OBJECT_TYPE_LAG, next_lag_id++, lag_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG OID\n");
        return status;
    }
    printf("CREATE LAG: 0x%lX\n", *lag_id);

    STUB_LOG_EXIT();
    return status;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    STUB_LOG_ENTER();

    printf("REMOVE LAG: 0x%lX\n", lag_id);

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
    STUB_LOG_ENTER();

    printf("GET LAG 0x%lX ATTRIBUTE(S):", lag_id);

    for (uint32_t i = 0; i < attr_count; ++i)
    {
        switch (attr_list[i].id)
        {
            case SAI_LAG_ATTR_PORT_LIST:
                printf(" PORT_LIST");
                break;
            default:
                printf(" <BAD_ATTR:%d>\n", attr_list[i].id);
                return SAI_STATUS_INVALID_ATTR_VALUE_0 + attr_list[i].id;
        }
    }
    printf("\n");

    STUB_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count, _In_ sai_attribute_t *attr_list) {
    static int32_t next_member_id = 1;
    sai_status_t status = SAI_STATUS_SUCCESS;
    STUB_LOG_ENTER();

    status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, next_member_id++, lag_member_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create LAG member OID\n");
        return status;
    }
    printf("CREATE LAG MEMBER: 0x%lX\n", *lag_member_id);

    STUB_LOG_EXIT();
    return status;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
    STUB_LOG_ENTER();

    printf("REMOVE LAG MEMBER: 0x%lX\n", lag_member_id);

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
    STUB_LOG_ENTER();

    printf("GET LAG MEMBER 0x%lX ATTRIBUTE(S):", lag_member_id);

    for (uint32_t i = 0; i < attr_count; ++i)
    {
        switch (attr_list[i].id)
        {
            case SAI_LAG_MEMBER_ATTR_LAG_ID:
                printf(" LAG_ID");
                break;
            case SAI_LAG_MEMBER_ATTR_PORT_ID:
                printf(" PORT_ID");
                break;
            default:
                printf(" <ATTR:%d>", attr_list[i].id);
        }
    }
    printf("\n");

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
