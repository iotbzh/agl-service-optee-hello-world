#include <wrap-json.h>
#include <afb/afb-binding.h>
#include <json-c/json.h>

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>
/* To the the UUID (found the the TA's h-file(s)) */
#include <hello_world_ta.h>

static void OPTEE_INC(afb_req_t req){
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
	uint32_t err_origin;

    json_object *tmpJ;
    json_object *queryJ = afb_req_json(req);

    json_bool success = json_object_object_get_ex(queryJ, "value", &tmpJ);
	if (!success) {
		afb_req_fail_f(req, "ERROR", "key value not found in '%s'", json_object_get_string(queryJ));
		return;
	}
	if (json_object_get_type(tmpJ) != json_type_int) {
		afb_req_fail(req, "ERROR", "key value not an int");
		return;
	}

	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = json_object_get_int(tmpJ);

	printf("Invoking TA to increment %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_INC_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

    afb_req_success(req, json_object_new_int(op.params[0].value.a), NULL);
	printf("TA incremented value to %d\n", op.params[0].value.a);

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return;
}

static void OPTEE_DEC(afb_req_t req){
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
	uint32_t err_origin;

    json_object *tmpJ;
    json_object *queryJ = afb_req_json(req);

    json_bool success = json_object_object_get_ex(queryJ, "value", &tmpJ);
	if (!success) {
		afb_req_fail_f(req, "ERROR", "key value not found in '%s'", json_object_get_string(queryJ));
		return;
	}
	if (json_object_get_type(tmpJ) != json_type_int) {
		afb_req_fail(req, "ERROR", "key value not an int");
		return;
	}

	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	memset(&op, 0, sizeof(op));

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = json_object_get_int(tmpJ);

	printf("Invoking TA to increment %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_DEC_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

    afb_req_success(req, json_object_new_int(op.params[0].value.a), NULL);
	printf("TA decremented value to %d\n", op.params[0].value.a);

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

    return;
}

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                          BINDING INITIALIZATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
static int initialization(afb_api_t api)
{
    AFB_API_NOTICE(api, "Initialization");

    /* Variable declaration */
    int* userdata;

    /* Userdata initialization */
    userdata = calloc(sizeof(int),1);
    if (!userdata)
        return -1;

    /* Establish the link between tha api and the userdata */
    afb_api_set_userdata(api, userdata);

    /* Userdata initialization */
    AFB_API_NOTICE(api, "Initialization finished");
    return 0;
}

// Binder preinitialization
int preinit(afb_api_t api)
{
    AFB_API_NOTICE(api, "Preinitialization");

    // Set the verb of this API
    afb_api_add_verb(api, "inc", "call incrementation from hello world TA", OPTEE_INC, NULL, NULL, AFB_SESSION_NONE, 0);
    afb_api_add_verb(api, "dec", "call decrementation from hello world TA", OPTEE_DEC, NULL, NULL, AFB_SESSION_NONE, 0);

    // Define the initialization function
    afb_api_on_init(api, initialization);

    // Now the api is set and ready to work
    afb_api_seal(api);

    AFB_API_NOTICE(api, "Preinitialization finished");

    return 0;
}

const afb_binding_t afbBindingExport = {
    .api = "optee-hello-world",
    .specification = NULL,
    .preinit = preinit,
    .provide_class = NULL,
    .require_class = NULL,
    .require_api = NULL,
    .noconcurrency = 0
};
