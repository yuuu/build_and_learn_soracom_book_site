package main

import (
	sdk "github.com/soracom/orbit-sdk-tinygo"
	"github.com/yuuu/build_and_learn_soracom_book_site/chapter4/section4-2/orbit/data"
	"github.com/yuuu/build_and_learn_soracom_book_site/chapter4/section4-2/orbit/errs"
)

func main() {
}

//export uplink
func uplink() sdk.ErrorCode {
	inputBuffer, err := sdk.GetInputBuffer()
	if err != nil {
		sdk.Log(err.Error())
		return sdk.ErrorCode(errs.ExecErrorCode)
	}

	output, err := convertInputToOutput(inputBuffer)
	if err != nil {
		sdk.Log(err.Error())
		return sdk.ErrorCode(errs.ExecErrorCode)
	}

	serializedOutput, err := output.SerializeJSON()
	if err != nil {
		sdk.Log(err.Error())
		return sdk.ErrorCode(errs.ExecErrorCode)
	}

	sdk.SetOutputJSON(string(serializedOutput))

	return sdk.ErrorCode(errs.OkErrorCode)
}

func convertInputToOutput(input []byte) (*data.Output, error) {
	sdk.Log(string(input))
	return &data.Output{
		Message: string(input),
	}, nil
}
