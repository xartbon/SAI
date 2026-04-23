#include <stdio.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
    sai_status_t              status;
    sai_lag_api_t            *lag_api;
    sai_object_id_t           lag_id[2];
    sai_object_id_t           lag_member_id[4];

    status = sai_api_initialize(0, &test_services);
    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to query LAG API, status=%d\n", status);
        return 1;
    }

    /*
     * - Create LAG#1
     * - Create LAG_MEMBER#1 {LAG_ID:LAG#1, PORT_ID:PORT#1}
     * - Create LAG_MEMBER#2 {LAG_ID:LAG#1, PORT_ID:PORT#2}
     */
    status = lag_api->create_lag(&lag_id[0], 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG, status=%d\n", status);
        return 1;
    }

    sai_attribute_t lag_member_1_attributes[] = {
        { .id = SAI_LAG_MEMBER_ATTR_LAG_ID, .value.oid = lag_id[0] },
        { .id = SAI_LAG_MEMBER_ATTR_PORT_ID, .value.oid = 0x0 /* ignore for now */ },
    };
    status = lag_api->create_lag_member(&lag_member_id[0], sizeof(lag_member_1_attributes) / sizeof(sai_attribute_t), (sai_attribute_t*) &lag_member_1_attributes);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG member, status=%d\n", status);
        return 1;
    }

    sai_attribute_t lag_member_2_attributes[] = {
        { .id = SAI_LAG_MEMBER_ATTR_LAG_ID, .value.oid = lag_id[0] },
        { .id = SAI_LAG_MEMBER_ATTR_PORT_ID, .value.oid = 0x0 /* ignore for now */ },
    };
    status = lag_api->create_lag_member(&lag_member_id[1], sizeof(lag_member_2_attributes) / sizeof(sai_attribute_t), (sai_attribute_t*) &lag_member_2_attributes);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG member, status=%d\n", status);
        return 1;
    }

    /*
     * - Create LAG#2
     * - Create LAG_MEMBER#3 {LAG_ID:LAG#2, PORT_ID:PORT#3}
     * - Create LAG_MEMBER#4 {LAG_ID:LAG#2, PORT_ID:PORT#4}
     */
    status = lag_api->create_lag(&lag_id[1], 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG, status=%d\n", status);
        return 1;
    }

    sai_attribute_t lag_member_3_attributes[] = {
        { .id = SAI_LAG_MEMBER_ATTR_LAG_ID, .value.oid = lag_id[1] },
        { .id = SAI_LAG_MEMBER_ATTR_PORT_ID, .value.oid = 0x0 /* ignore for now */ },
    };
    status = lag_api->create_lag_member(&lag_member_id[2], sizeof(lag_member_1_attributes) / sizeof(sai_attribute_t), (sai_attribute_t*) &lag_member_1_attributes);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG member, status=%d\n", status);
        return 1;
    }

    sai_attribute_t lag_member_4_attributes[] = {
        { .id = SAI_LAG_MEMBER_ATTR_LAG_ID, .value.oid = lag_id[1] },
        { .id = SAI_LAG_MEMBER_ATTR_PORT_ID, .value.oid = 0x0 /* ignore for now */ },
    };
    status = lag_api->create_lag_member(&lag_member_id[3], sizeof(lag_member_2_attributes) / sizeof(sai_attribute_t), (sai_attribute_t*) &lag_member_2_attributes);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create LAG member, status=%d\n", status);
        return 1;
    }

    /*
     * - Get LAG#1 PORT_LIST [Expected: (PORT#1, PORT#2)]
     * - Get LAG#2 PORT_LIST [Expected: (PORT#3, PORT#4)]
     */
    sai_attribute_t lag_attribute = {
        .id = SAI_LAG_ATTR_PORT_LIST
    };

    status = lag_api->get_lag_attribute(lag_id[0], 1, &lag_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve LAG port list, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lag_id[1], 1, &lag_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve LAG port list, status=%d\n", status);
        return 1;
    }

    /*
     * - Get LAG_MEMBER#1 LAG_ID [Expected: LAG#1]
     * - Get LAG_MEMBER#3 PORT_ID [Expected: PORT#3]
     */
    sai_attribute_t lag_member_attribute = {
        .id = SAI_LAG_MEMBER_ATTR_LAG_ID
    };

    status = lag_api->get_lag_member_attribute(lag_member_id[0], 1, &lag_member_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve attribute from LAG member, status=%d\n", status);
        return 1;
    }

    lag_member_attribute.id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member_attribute.value.oid = 0;  

    status = lag_api->get_lag_member_attribute(lag_member_id[2], 1, &lag_member_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve attribute from LAG member, status=%d\n", status);
        return 1;
    }

    /*
     * - Remove LAG_MEMBER#2
     * - Get LAG#1 PORT_LIST [Expected: (PORT#1)]
     */
    status = lag_api->remove_lag_member(lag_member_id[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, status=%d\n", status);
        return 1;
    }
    status = lag_api->get_lag_attribute(lag_id[0], 1, &lag_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve LAG port list, status=%d\n", status);
        return 1;
    }

    /*
     * - Remove LAG_MEMBER#3
     * - Get LAG#2 PORT_LIST [Expected: (PORT#4)]
     */
    status = lag_api->remove_lag_member(lag_member_id[2]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, status=%d\n", status);
        return 1;
    }
    status = lag_api->get_lag_attribute(lag_id[1], 1, &lag_attribute);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Failed to retrieve LAG port list, status=%d\n", status);
        return 1;
    }

    /*
     * - Remove LAG_MEMBER#1
     * - Remove LAG_MEMBER#4
     * - Remove LAG#2
     * - Remove LAG#1
     */
    status = lag_api->remove_lag_member(lag_member_id[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lag_member_id[3]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag_id[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag_id[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove LAG, status=%d\n", status);
        return 1;
    }

    status = sai_api_uninitialize();
    return 0;
}
